/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  Dhad IDE — معالج ض (تصميم خشبي فاتح)
 *  محرر + مجمّع + محاكي + مصحح أخطاء
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QSplitter>
#include <QGroupBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include <QHeaderView>
#include <QFont>
#include <QFile>
#include <QTextStream>
#include <QSpinBox>
#include <QPalette>
#include <QFrame>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QScrollArea>
#include <QTabWidget>
#include <QProcess>
#include <QDir>
#include <QFileInfo>
#include <QSet>
#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextBlock>
#include <QTextCharFormat>
#include <cstdio>
#include <vector>
#include <string>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <cmath>

extern "C" {
#include "dhad_asm.h"
}

#include <signal.h>
#include <execinfo.h>

static void crash_handler(int sig){
    void *array[20];
    int size = backtrace(array, 20);
    FILE *f = fopen("/tmp/dhad_crash.log", "w");
    if(f){
        fprintf(f, "Signal %d\n", sig);
        backtrace_symbols_fd(array, size, fileno(f));
        fclose(f);
    }
    _exit(1);
}

using namespace std;

/* ═══════════════════════════════════════════════════════════════════════════════
 *  ثوابت المعالج
 * ═══════════════════════════════════════════════════════════════════════════════ */

constexpr int MEM_SIZE   = 65536;
constexpr int PROG_START = 0x0000;
constexpr int STACK_SIZE = 32;

constexpr uint8_t OP_HALT=0x0,OP_LOAD=0x1,OP_MOV=0x2,OP_LDMEM=0x3,
                 OP_STMEM=0x4,OP_PUSH=0x5,OP_POP=0x6,OP_CALL=0x7,
                 OP_RET=0x8,OP_ADD=0x9,OP_SUB=0xA,OP_MUL=0xB,
                 OP_DIV=0xC,OP_MOD=0xD,OP_PRINT=0xE,OP_EXT=0xF;

/* ═══════════════════════════════════════════════════════════════════════════════
 *  هيكل المعالج
 * ═══════════════════════════════════════════════════════════════════════════════ */

struct CPU {
    uint8_t  regs[8]={};uint8_t acc=0,flags=0,sp=0;
    uint16_t pc=PROG_START;
    uint8_t  mem[MEM_SIZE]={};
    uint16_t stack[STACK_SIZE]={};
    int      stack_top=-1;
    bool     halted=false;
    uint32_t cycle=0,inst_count=0;
    string   output="";
    uint16_t prog_size=0;
    bool     int_enabled=false;
    uint8_t  int_mask=0xFF;
    uint8_t  int_pending=0;
    uint16_t ivt_addr=0x0040;
};

static const char* op_name(uint8_t o){
    switch(o){case 0:return"tawqif";case 1:return"hamil";case 2:return"naql";
    case 3:return"iqra";case 4:return"akhzin";case 5:return"idfa";
    case 6:return"ashab";case 7:return"nida";case 8:return"irja";
    case 9:return"jam";case 0xA:return"tarkh";case 0xB:return"darb";
    case 0xC:return"qasma";case 0xD:return"baqi";case 0xE:return"tabi";
    case 0xF:return"mawsa";default:return"???";}
}

static uint8_t fetch(CPU& c){uint8_t b=c.mem[c.pc];c.pc++;return b;}
static uint16_t fetch16(CPU& c){uint8_t lo=fetch(c),hi=fetch(c);return(hi<<8)|lo;}

static uint8_t gui_get_reg(CPU& c,uint8_t r){return(r==8)?c.acc:c.regs[r&7];}
static void gui_set_reg(CPU& c,uint8_t r,uint8_t v){if(r==8)c.acc=v;else c.regs[r&7]=v;}

static void cpu_step(CPU& c){
    if(c.halted)return;
    if(c.int_enabled){
        uint8_t pending=c.int_pending&c.int_mask;
        if(pending){
            for(int i=0;i<8;i++){
                if(pending&(1<<i)){
                    c.int_pending&=~(1<<i);
                    if(c.sp<STACK_SIZE-1){c.stack[c.sp++]=c.flags;c.stack[c.sp++]=c.pc;}
                    c.int_enabled=false;
                    c.pc=c.ivt_addr+(i*4);
                    c.cycle+=3;return;
                }
            }
        }
    }
    uint8_t raw=fetch(c),opc=(raw>>4)&0xF,reg=raw&0xF;
    c.cycle++;c.inst_count++;
    switch(opc){
    case 0x0:break;
    case 0x1:{uint8_t val=fetch(c);if(reg<8)c.regs[reg]=val;else c.acc=val;break;}
    case 0x2:{uint8_t dst=fetch(c);gui_set_reg(c,dst,gui_get_reg(c,reg));break;}
    case 0x3:{uint8_t a=fetch(c),v=c.mem[a];if(reg<8)c.regs[reg]=v;else c.acc=v;break;}
    case 0x4:{uint8_t a=fetch(c);c.mem[a]=(reg<8)?c.regs[reg]:c.acc;break;}
    case 0x5:if(c.sp<STACK_SIZE){c.stack[c.sp]=c.acc;c.stack_top=c.sp;c.sp++;}break;
    case 0x6:if(c.sp>0){c.sp--;c.acc=c.stack[c.sp]&0xFF;c.stack_top=c.sp-1;}break;
    case 0x7:{uint16_t a=fetch16(c);if(c.sp<STACK_SIZE){c.stack[c.sp]=c.pc;c.stack_top=c.sp;c.sp++;c.pc=a;}break;}
    case 0x8:if(reg==0){if(c.sp>0){c.sp--;c.pc=c.stack[c.sp];c.stack_top=c.sp-1;}}else{c.acc=(uint8_t)(-(int8_t)c.acc);c.flags=0;if(!c.acc)c.flags|=1;if(c.acc&0x80)c.flags|=2;}break;
    case 0x9:{uint8_t v=gui_get_reg(c,reg);uint16_t s=c.acc+v;c.acc=s&0xFF;c.flags=0;if(!c.acc)c.flags|=1;if(c.acc&0x80)c.flags|=2;if(s>0xFF)c.flags|=4;}break;
    case 0xA:{uint8_t v=gui_get_reg(c,reg);int s=c.acc-v;c.flags=0;c.acc=s&0xFF;if(!c.acc)c.flags|=1;if(c.acc&0x80)c.flags|=2;if(s<0)c.flags|=4;}break;
    case 0xB:{uint8_t v=gui_get_reg(c,reg);c.acc=(c.acc*v)&0xFF;c.flags=0;if(!c.acc)c.flags|=1;if(c.acc&0x80)c.flags|=2;}break;
    case 0xC:{uint8_t v=gui_get_reg(c,reg);if(v){c.acc/=v;c.flags=0;if(!c.acc)c.flags|=1;if(c.acc&0x80)c.flags|=2;}break;}
    case 0xD:{uint8_t v=gui_get_reg(c,reg);if(v){c.acc%=v;c.flags=0;if(!c.acc)c.flags|=1;if(c.acc&0x80)c.flags|=2;}break;}
    case 0xE:c.output+=to_string(c.acc);break;
    case 0xF:{uint8_t ex=fetch(c);
        if(ex<=0x40){uint16_t a=fetch16(c);bool j=false;
            switch(ex){case 0x00:j=true;break;case 0x10:j=c.flags&1;break;case 0x20:j=!(c.flags&1);break;
            case 0x30:j=c.flags&4;break;case 0x40:j=c.flags&2;break;}if(j)c.pc=a;}
        else if(ex==0x50){c.output+=(char)c.acc;}
        else if(ex==0x60){c.acc=rand()%256;}
        else if(ex==0xD3){c.halted=true;}
        else if(ex==0xD0){uint8_t op2=fetch(c);uint8_t dst=(op2>>4)&0xF,src=op2&0xF;
            uint8_t *pd=(dst<8)?&c.regs[dst]:&c.acc;uint8_t *ps=(src<8)?&c.regs[src]:&c.acc;
            uint8_t tmp=*pd;*pd=*ps;*ps=tmp;}
        else if(ex==0xD1){uint8_t r=fetch(c)&0xF;uint8_t v=gui_get_reg(c,r);v--;gui_set_reg(c,r,v);c.flags=0;if(!v)c.flags|=1;if(v&0x80)c.flags|=2;}
        else if(ex==0xD2){uint8_t r=fetch(c)&0xF;uint8_t v=gui_get_reg(c,r);v++;gui_set_reg(c,r,v);c.flags=0;if(!v)c.flags|=1;if(v&0x80)c.flags|=2;}
        else if((ex&0xF0)==0xE0){uint8_t v=gui_get_reg(c,ex&0xF);uint16_t r=c.acc-v;c.flags=0;if(!(r&0xFF))c.flags|=1;if(r&0x80)c.flags|=2;if(r>0xFF)c.flags|=4;}
        else if(ex==0xE1){c.int_enabled=true;}
        else if(ex==0xE2){c.int_enabled=false;}
        else if(ex==0xE3){if(c.sp>=2){c.pc=c.stack[--c.sp];c.flags=(uint8_t)(c.stack[--c.sp]&0xFF);c.int_enabled=true;}}
        else{switch(ex&0xF0){
            case 0x70:c.acc^=gui_get_reg(c,ex&0xF);break;case 0x80:c.acc|=gui_get_reg(c,ex&0xF);break;
            case 0x90:c.acc&=gui_get_reg(c,ex&0xF);break;case 0xA0:c.acc=~c.acc;break;
            case 0xB0:{c.flags=0;if(c.acc&0x80)c.flags|=4;c.acc<<=1;break;}
            case 0xC0:{c.flags=0;if(c.acc&1)c.flags|=4;c.acc>>=1;break;}}
            c.flags&=~3;if(!c.acc)c.flags|=1;if(c.acc&0x80)c.flags|=2;}
        break;}
    default:break;
    }
}

static int load_bin(CPU& c,const QByteArray& data){
    if(data.isEmpty() || data.size()>0xF000)return -1;
    memset(&c.mem[0],0,MEM_SIZE);
    memcpy(&c.mem[PROG_START],data.constData(),data.size());
    c.prog_size=(uint16_t)data.size();c.pc=PROG_START;
    c.halted=false;c.cycle=0;c.inst_count=0;c.sp=0;c.stack_top=-1;
    c.output="";c.flags=0;c.acc=0;
    memset(c.regs,0,8);memset(c.stack,0,STACK_SIZE*sizeof(uint16_t));
    return data.size();
}

/* ═══════════════════════════════════════════════════════════════════════════════
 *  ثوابت التصميم — Wood Light Theme
 * ═══════════════════════════════════════════════════════════════════════════════ */

constexpr const char* CLR_BG         = "#FFFBF7";  // خلفية رئيسية — أبيض كريمي
constexpr const char* CLR_BG_WARM    = "#F5EDE4";  // خلفية دافئة — بيج
constexpr const char* CLR_BG_CARD    = "#EDE3D8";  // بطاقات — بيج غامق قليلاً
constexpr const char* CLR_BG_INPUT   = "#FFFFFF";  // حقول الإدخال — أبيض نقي
constexpr const char* CLR_BG_HOVER   = "#E8DDD0";  // تمرير — بيج غامق
constexpr const char* CLR_BG_SEL     = "#D4C4B0";  // تحديد — بني فاتح

constexpr const char* CLR_BORDER     = "#C8B8A4";  // حدود — بني فاتح
constexpr const char* CLR_BORDER_DK  = "#A08A70";  // حدود غامقة — بني متوسط
constexpr const char* CLR_LINE       = "#B8A894";  // خطوط الفصل

constexpr const char* CLR_TEXT       = "#2C1810";  // نص أساسي — بني غامق جداً
constexpr const char* CLR_TEXT_SEC   = "#5C3D28";  // نص ثانوي — بني متوسط
constexpr const char* CLR_TEXT_DIM   = "#8B6B4E";  // نص باهت — بني فاتح

// ألوان البني الخشبي — درجات متنوعة
constexpr const char* CLR_WOOD       = "#6B4226";  // بني خشبي — أساسي
constexpr const char* CLR_WOOD_LT    = "#8B5E3C";  // بني خشبي فاتح
constexpr const char* CLR_WOOD_DK    = "#3E2415";  // بني خشبي غامق
constexpr const char* CLR_WALNUT     = "#5C3317";  // جوزي
constexpr const char* CLR_OAK        = "#7A5230";  // بلوطي
constexpr const char* CLR_PINE       = "#A0704C"  ; // صنوبري
constexpr const char* CLR_CEDAR      = "#8B4513";  // أرز

// ألوان الوظائف
constexpr const char* CLR_ACTIVE     = "#5A7247";  // نشط — أخضر زيتوني
constexpr const char* CLR_ACTIVE_LT  = "#7A9560";  // نشط فاتح
constexpr const char* CLR_WARN       = "#B85C38";  // تحذير — برتقالي طوبي
constexpr const char* CLR_DANGER     = "#A04030";  // خطأ — أحمر بني
constexpr const char* CLR_ACCENT     = "#9B7340";  // تمييز — كهرماني غامق

// ألوان السجلات — درجات بني
constexpr const char* REG_COLORS[] = {
    "#6B4226", "#5A7247", "#B85C38", "#8B4513",
    "#7A5230", "#A0704C", "#3E2415", "#9B7340"
};

// تنسيقات الأزرار
static QString btn_style(const char* bg, const char* bg_hover, const char* text = "#FFFFFF") {
    return QString(
        "QPushButton{"
        "background:%1;color:%4;border:none;border-radius:6px;"
        "padding:8px 18px;font:bold 12px 'Sans';"
        "}"
        "QPushButton:hover{background:%2;}"
        "QPushButton:pressed{background:%3;}"
    ).arg(bg).arg(bg_hover).arg(bg_hover).arg(text);
}

static const QString SHEET_BTN_PRIMARY  = btn_style("#6B4226", "#8B5E3C");
static const QString SHEET_BTN_SUCCESS  = btn_style("#5A7247", "#7A9560");
static const QString SHEET_BTN_DANGER   = btn_style("#A04030", "#B84838");
static const QString SHEET_BTN_NEUTRAL  = btn_style("#A08A70", "#B8A080", "#2C1810");

static const char* SHEET_GROUP =
    "QGroupBox{"
    "font:bold 11px 'Sans';"
    "color:#5C3D28;"
    "border:1px solid #C8B8A4;"
    "border-radius:8px;"
    "margin-top:10px;"
    "padding:14px 6px 6px 6px;"
    "background:#F5EDE4;"
    "}"
    "QGroupBox::title{"
    "subcontrol-origin:margin;"
    "left:12px;"
    "padding:0 8px;"
    "}";

static const char* SHEET_TABLE =
    "QTableWidget{"
    "background:#FFFBF7;"
    "color:#2C1810;"
    "border:1px solid #C8B8A4;"
    "alternate-background-color:#F5EDE4;"
    "gridline-color:#E0D4C4;"
    "font:11px 'Monospace';"
    "}"
    "QTableWidget::item:selected{"
    "background:#D4C4B0;"
    "color:#2C1810;"
    "}"
    "QHeaderView::section{"
    "background:#EDE3D8;"
    "color:#5C3D28;"
    "font:bold 10px 'Sans';"
    "border:none;"
    "border-bottom:2px solid #6B4226;"
    "padding:3px;"
    "}";

/* ═══════════════════════════════════════════════════════════════════════════════
 *  مؤشر الصيغة — ألوان خشبية فاتحة
 * ═══════════════════════════════════════════════════════════════════════════════ */

class DhadHighlighter : public QSyntaxHighlighter {
public:
    explicit DhadHighlighter(QTextDocument* parent) : QSyntaxHighlighter(parent) {}
    struct Rule { QRegularExpression pattern; QTextCharFormat fmt; };
    vector<Rule> rules;

    void setup_rules() {
        // تعليمات الحساب — بني غامق
        QTextCharFormat inst_fmt;
        inst_fmt.setForeground(QColor("#3E2415"));
        inst_fmt.setFontWeight(QFont::Bold);
        { Rule r; r.pattern = QRegularExpression(QString::fromUtf8("\\b(جمع|طرح|ضرب|قسمة|باقي|سالب)\\b")); r.fmt = inst_fmt; rules.push_back(r); }
        { Rule r; r.pattern = QRegularExpression("\\b(ADD|SUB|MUL|DIV|MOD|NEG)\\b"); r.fmt = inst_fmt; rules.push_back(r); }

        // تعليمات النقل — أخضر زيتوني
        QTextCharFormat mov_fmt;
        mov_fmt.setForeground(QColor("#5A7247"));
        mov_fmt.setFontWeight(QFont::Bold);
        { Rule r; r.pattern = QRegularExpression(QString::fromUtf8("\\b(حمّل|نقل|ادفع|اسحب|اكتب|اقرأ)\\b")); r.fmt = mov_fmt; rules.push_back(r); }
        { Rule r; r.pattern = QRegularExpression("\\b(LOAD|MOV|LDMEM|STMEM|PUSH|POP|PRINT)\\b"); r.fmt = mov_fmt; rules.push_back(r); }

        // تعليمات القفز — برتقالي طوبي
        QTextCharFormat jmp_fmt;
        jmp_fmt.setForeground(QColor("#B85C38"));
        jmp_fmt.setFontWeight(QFont::Bold);
        { Rule r; r.pattern = QRegularExpression(QString::fromUtf8("\\b(قفز|قفز_صفر|قفز_ليس_صفر|nida|irja)\\b")); r.fmt = jmp_fmt; rules.push_back(r); }
        { Rule r; r.pattern = QRegularExpression("\\b(JMP|JZ|JNZ|JC|JN|CALL|RET)\\b"); r.fmt = jmp_fmt; rules.push_back(r); }

        // تعليمات موسّعة — أرز
        QTextCharFormat ext_fmt;
        ext_fmt.setForeground(QColor("#8B4513"));
        ext_fmt.setFontWeight(QFont::Bold);
        { Rule r; r.pattern = QRegularExpression("\\b(SWAP|DEC|INC|HALT|CMP|EI|DI|RETI|NOP|XOR|OR|AND|NOT|SHL|SHR|RAND|PUTC)\\b"); r.fmt = ext_fmt; rules.push_back(r); }

        // سجلات — كهرماني غامق
        QTextCharFormat reg_fmt;
        reg_fmt.setForeground(QColor("#9B7340"));
        reg_fmt.setFontWeight(QFont::Bold);
        { Rule r; r.pattern = QRegularExpression(QString::fromUtf8("\\b(مح|س[0-7]|R[0-7]|ACC)\\b")); r.fmt = reg_fmt; rules.push_back(r); }

        // تعريفات بيانات — بلوطي
        QTextCharFormat data_fmt;
        data_fmt.setForeground(QColor("#7A5230"));
        { Rule r; r.pattern = QRegularExpression("\\.(db|dw|ds|org|align|macro|endm|if|else|endif|include|equ)\\b"); r.fmt = data_fmt; rules.push_back(r); }

        // تعليقات — رمادي بني
        QTextCharFormat cmt_fmt;
        cmt_fmt.setForeground(QColor("#A08A70"));
        cmt_fmt.setFontItalic(true);
        { Rule r; r.pattern = QRegularExpression(";[^\n]*"); r.fmt = cmt_fmt; rules.push_back(r); }

        // أرقام — صنوبري
        QTextCharFormat num_fmt;
        num_fmt.setForeground(QColor("#A0704C"));
        { Rule r; r.pattern = QRegularExpression("\\b0x[0-9A-Fa-f]+\\b"); r.fmt = num_fmt; rules.push_back(r); }
        { Rule r; r.pattern = QRegularExpression("\\b[0-9]+\\b"); r.fmt = num_fmt; rules.push_back(r); }
    }

protected:
    void highlightBlock(const QString& text) override {
        if (rules.empty()) setup_rules();
        for (const auto& rule : rules) {
            QRegularExpressionMatchIterator it = rule.pattern.globalMatch(text);
            while (it.hasNext()) {
                QRegularExpressionMatch m = it.next();
                setFormat(m.capturedStart(), m.capturedLength(), rule.fmt);
            }
        }
    }
};

/* ═══════════════════════════════════════════════════════════════════════════════
 *  محرر الكود
 * ═══════════════════════════════════════════════════════════════════════════════ */

class CodeEditor : public QPlainTextEdit {
    Q_OBJECT
public:
    explicit CodeEditor(QWidget* parent = nullptr) : QPlainTextEdit(parent) {
        setMinimumWidth(300);
        QFont mono("Monospace", 13);
        mono.setStyleHint(QFont::Monospace);
        setFont(mono);
        setTabStopDistance(QFontMetricsF(mono).horizontalAdvance(' ') * 4);
        setLineWrapMode(QPlainTextEdit::NoWrap);
        line_area = new LineArea(this);
        connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::update_line_area_width);
        connect(this, &CodeEditor::updateRequest, this, &CodeEditor::update_line_area);
        update_line_area_width(0);
        setStyleSheet(QString(
            "QPlainTextEdit {"
            "background: %1; color: %2;"
            "border: 1px solid %3; border-radius: 6px;"
            "padding: 8px; selection-background-color: #D4C4B0;"
            "}"
        ).arg(CLR_BG_INPUT, CLR_TEXT, CLR_BORDER));
    }

    void set_breakpoint(int line) {
        if (breakpoints.contains(line)) breakpoints.remove(line);
        else breakpoints.insert(line);
        viewport()->update();
    }

    int line_area_width() {
        int digits = 1, max = qMax(1, blockCount());
        while (max >= 10) { max /= 10; ++digits; }
        return 14 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits + 20;
    }
    void update_line_area_width(int) { setViewportMargins(line_area_width(), 0, 0, 0); }
    void update_line_area(const QRect& rect, int dy) {
        if (dy) line_area->scroll(0, dy);
        else line_area->update(0, rect.y(), line_area->width(), rect.height());
        if (rect.contains(viewport()->rect())) update_line_area_width(0);
    }

protected:
    void resizeEvent(QResizeEvent* e) override {
        QPlainTextEdit::resizeEvent(e);
        QRect cr = contentsRect();
        line_area->setGeometry(QRect(cr.left(), cr.top(), line_area_width(), cr.height()));
    }
    void keyPressEvent(QKeyEvent* e) override {
        if (e->key() == Qt::Key_F9) { set_breakpoint(textCursor().blockNumber() + 1); return; }
        QPlainTextEdit::keyPressEvent(e);
    }
    void mousePressEvent(QMouseEvent* e) override {
        if (e->pos().x() < line_area_width() - 10) {
            set_breakpoint(cursorForPosition(e->pos()).blockNumber() + 1); return;
        }
        QPlainTextEdit::mousePressEvent(e);
    }

private slots:
    void line_area_paint_event(QPaintEvent* event) {
        QPainter painter(line_area);
        painter.fillRect(event->rect(), QColor("#EDE3D8"));
        QTextBlock block = firstVisibleBlock();
        int block_num = block.blockNumber();
        int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
        int bottom = top + qRound(blockBoundingRect(block).height());
        while (block.isValid() && top <= event->rect().bottom()) {
            if (block.isVisible() && bottom >= event->rect().top()) {
                int line_num = block_num + 1;
                if (breakpoints.contains(line_num)) {
                    painter.setPen(Qt::NoPen);
                    painter.setBrush(QColor("#B85C38"));
                    painter.drawEllipse(QPointF(12, top + 12), 5, 5);
                    QRadialGradient glow(QPointF(12, top + 12), 9);
                    glow.setColorAt(0, QColor(184,92,56,60));
                    glow.setColorAt(1, QColor(184,92,56,0));
                    painter.setBrush(glow);
                    painter.drawEllipse(QPointF(12, top + 12), 9, 9);
                }
                painter.setPen(breakpoints.contains(line_num) ? QColor("#B85C38") : QColor("#A08A70"));
                painter.setFont(font());
                painter.drawText(0, top, line_area->width() - 6, fontMetrics().height(),
                                 Qt::AlignRight | Qt::AlignVCenter, QString::number(line_num));
            }
            block = block.next(); top = bottom;
            bottom = top + qRound(blockBoundingRect(block).height());
            ++block_num;
        }
    }

private:
    class LineArea : public QWidget {
    public:
        CodeEditor* editor;
        LineArea(CodeEditor* e) : QWidget(e), editor(e) {}
        QSize sizeHint() const override { return QSize(editor->line_area_width(), 0); }
    protected:
        void paintEvent(QPaintEvent* e) override { editor->line_area_paint_event(e); }
    };
    LineArea* line_area;
    QSet<int> breakpoints;
};

/* ═══════════════════════════════════════════════════════════════════════════════
 *  رسم بياني متحرك للسجلات
 * ═══════════════════════════════════════════════════════════════════════════════ */

class RegisterBar : public QWidget {
    Q_OBJECT
    Q_PROPERTY(int animVal READ animVal WRITE setAnimVal)
public:
    explicit RegisterBar(QWidget* parent=nullptr):QWidget(parent){
        setMinimumHeight(42);setMinimumWidth(80);
    }
    int animVal()const{return current_val;}
    void setAnimVal(int v){current_val=v;update();}
    void setValue(int v){
        target_val=v;
        if(anim)anim->stop();
        anim=new QPropertyAnimation(this,"animVal");
        anim->setDuration(250);anim->setStartValue(current_val);anim->setEndValue(v);
        anim->setEasingCurve(QEasingCurve::OutCubic);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }
    void setLabel(const QString& l){label=l;update();}
    void setColor(const QColor& c){bar_color=c;update();}
protected:
    void paintEvent(QPaintEvent*)override{
        QPainter p(this);p.setRenderHint(QPainter::Antialiasing);
        int w=width(),h=height();
        // خلفية
        p.setPen(QPen(QColor(CLR_BORDER),1));
        p.setBrush(QColor(CLR_BG_WARM));
        p.drawRoundedRect(0,0,w,h,6,6);
        // شريط علوي بلون السجل
        QPainterPath clip;
        clip.addRoundedRect(QRectF(0,0,w,h),6,6);
        p.setClipPath(clip);
        p.setPen(Qt::NoPen);
        p.setBrush(bar_color);
        p.drawRect(0,0,w,3);
        p.setClipping(false);
        // شريط القيمة
        double pct=(double)current_val/255.0;
        int bar_w=(int)(pct*(w-16));
        QLinearGradient grad(6,h/2-4,6+bar_w,h/2-4);
        grad.setColorAt(0,bar_color.lighter(140));
        grad.setColorAt(1,bar_color);
        p.setBrush(grad);
        p.drawRoundedRect(6,h/2-4,qMax(0,bar_w),8,3,3);
        // نص الاسم
        p.setPen(QColor(CLR_TEXT));
        p.setFont(QFont("Sans",9,QFont::Bold));
        p.drawText(QRect(6,2,w-12,h/2-4),Qt::AlignLeft|Qt::AlignVCenter,label);
        // نص القيمة
        p.setPen(bar_color);
        p.setFont(QFont("Monospace",8));
        p.drawText(QRect(6,h/2+6,w-12,h/2-6),Qt::AlignLeft|Qt::AlignVCenter,
                   QString("0x%1").arg(current_val,2,16,QChar('0')));
    }
private:
    int target_val=0,current_val=0;QString label;
    QColor bar_color=QColor("#6B4226");QPropertyAnimation* anim=nullptr;
};

/* ═══════════════════════════════════════════════════════════════════════════════
 *  مؤشر الأعلام
 * ═══════════════════════════════════════════════════════════════════════════════ */

class FlagIndicator : public QWidget {
    Q_OBJECT
    Q_PROPERTY(qreal glow READ glow WRITE setGlow)
public:
    FlagIndicator(const QString& l,const QColor& c,QWidget* parent=nullptr)
        :QWidget(parent),label(l),color(c){setFixedSize(52,52);}
    qreal glow()const{return glow_val;}
    void setGlow(qreal v){glow_val=v;update();}
    void setOn(bool on){
        target_glow=on?1.0:0.0;
        if(anim)anim->stop();
        anim=new QPropertyAnimation(this,"glow");
        anim->setDuration(200);anim->setStartValue(glow_val);anim->setEndValue(target_glow);
        anim->setEasingCurve(QEasingCurve::OutCubic);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }
protected:
    void paintEvent(QPaintEvent*)override{
        QPainter p(this);p.setRenderHint(QPainter::Antialiasing);
        double cx=width()/2.0,cy=height()/2.0-2;
        p.setPen(Qt::NoPen);
        // دائرة خارجية
        p.setBrush(QColor(CLR_BG_WARM));
        p.setPen(QPen(QColor(glow_val>0.1 ? color : QColor(CLR_BORDER)),2));
        p.drawEllipse(QPointF(cx,cy),12,12);
        // نقطة داخلية
        p.setPen(Qt::NoPen);
        p.setBrush(glow_val>0.1 ? color : QColor(CLR_BORDER));
        p.drawEllipse(QPointF(cx,cy),5,5);
        // توهج
        if(glow_val>0.3){
            QRadialGradient glow(QPointF(cx,cy),14);
            glow.setColorAt(0,QColor(color.red(),color.green(),color.blue(),(int)(80*glow_val)));
            glow.setColorAt(1,QColor(color.red(),color.green(),color.blue(),0));
            p.setBrush(glow);p.drawEllipse(QPointF(cx,cy),14,14);
        }
        // نص
        p.setPen(QColor(glow_val>0.1 ? color : QColor(CLR_TEXT_DIM)));
        p.setFont(QFont("Sans",8,QFont::Bold));
        p.drawText(QRect(0,height()-14,width(),12),Qt::AlignCenter,label);
    }
private:
    QString label;QColor color;qreal glow_val=0,target_glow=0;QPropertyAnimation* anim=nullptr;
};

/* ═══════════════════════════════════════════════════════════════════════════════
 *  صندوق الحالة
 * ═══════════════════════════════════════════════════════════════════════════════ */

class StateBox : public QFrame {
    Q_OBJECT
public:
    StateBox(const QString& title,QWidget* parent=nullptr):QFrame(parent){
        setFrameShape(QFrame::NoFrame);
        setStyleSheet(QString("StateBox{background:%1;border:1px solid %2;border-radius:8px;padding:4px;}")
                      .arg(CLR_BG_INPUT,CLR_BORDER));
        QVBoxLayout* lay=new QVBoxLayout(this);lay->setContentsMargins(10,6,10,6);lay->setSpacing(2);
        lbl_title=new QLabel(title);
        lbl_title->setStyleSheet(QString("color:%1;font:bold 9px 'Sans';border:none;letter-spacing:1px;").arg(CLR_TEXT_DIM));
        lbl_val=new QLabel("0x000");
        lbl_val->setStyleSheet(QString("color:%1;font:bold 18px 'Monospace';border:none;").arg(CLR_WOOD));
        lay->addWidget(lbl_title);lay->addWidget(lbl_val);
    }
    void setValue(const QString& v){lbl_val->setText(v);}
    void setValColor(const QString& c){lbl_val->setStyleSheet(QString("color:%1;font:bold 18px 'Monospace';border:none;").arg(c));}
private:
    QLabel* lbl_title;QLabel* lbl_val;
};

/* ═══════════════════════════════════════════════════════════════════════════════
 *  عداد دائري
 * ═══════════════════════════════════════════════════════════════════════════════ */

class CircularGauge : public QWidget {
    Q_OBJECT
    Q_PROPERTY(int gaugeVal READ gaugeVal WRITE setGaugeVal)
public:
    explicit CircularGauge(QWidget* parent=nullptr):QWidget(parent){setMinimumSize(90,90);}
    int gaugeVal()const{return current_val;}
    void setGaugeVal(int v){current_val=v;update();}
    void setTarget(int v){
        target_val=v;
        if(anim)anim->stop();
        anim=new QPropertyAnimation(this,"gaugeVal");
        anim->setDuration(350);anim->setStartValue(current_val);anim->setEndValue(v);
        anim->setEasingCurve(QEasingCurve::OutBack);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }
    void setLabel(const QString& l){lbl=l;update();}
    void setMax(int m){max_val=m;}
    void setColor(const QColor& c){bar_color=c;}
protected:
    void paintEvent(QPaintEvent*)override{
        QPainter p(this);p.setRenderHint(QPainter::Antialiasing);
        int s=min(width(),height());double cx=s/2.0,cy=s/2.0,r=s/2.0-10;
        // خلفية
        p.setPen(Qt::NoPen);p.setBrush(QColor(CLR_BG_WARM));
        p.setPen(QPen(QColor(CLR_BORDER),1));
        p.drawEllipse(QPointF(cx,cy),r+6,r+6);
        // حلقة رمادية
        QPen bg_pen(QColor(CLR_BORDER),5);
        bg_pen.setCapStyle(Qt::RoundCap);p.setPen(bg_pen);
        QRectF arc_rect(cx-r,cy-r,r*2,r*2);p.drawArc(arc_rect,0,360*16);
        // حلقة ملونة
        double pct=max_val>0?(double)current_val/(double)max_val:0;
        int span=(int)(pct*360*16);
        QPen fg_pen(bar_color,5);fg_pen.setCapStyle(Qt::RoundCap);p.setPen(fg_pen);
        p.drawArc(arc_rect,90*16,-span);
        // رقم
        p.setPen(QColor(CLR_TEXT));
        p.setFont(QFont("Sans",11,QFont::Bold));
        p.drawText(QRectF(cx-r,cy-14,r*2,28),Qt::AlignCenter,QString::number(current_val));
        // تسمية
        p.setPen(QColor(CLR_TEXT_DIM));
        p.setFont(QFont("Sans",8));
        p.drawText(QRectF(cx-r,cy+10,r*2,18),Qt::AlignCenter,lbl);
    }
private:
    int target_val=0,current_val=0,max_val=100000;
    QString lbl;QColor bar_color=QColor("#6B4226");QPropertyAnimation* anim=nullptr;
};

/* ═══════════════════════════════════════════════════════════════════════════════
 *  النافذة الرئيسية
 * ═══════════════════════════════════════════════════════════════════════════════ */

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent=nullptr):QMainWindow(parent){
        cpu=new CPU();*cpu=CPU{};cpu->pc=PROG_START;
        setup_ui();setupMenus();
        update_all();
        setWindowTitle(QString::fromUtf8("ض — Dhad IDE v4.0"));
        setMinimumSize(1200,750);resize(1500,900);
    }
    ~MainWindow(){run_timer->stop();delete cpu;}

protected:
    void keyPressEvent(QKeyEvent* e)override{
        if(e->modifiers()==Qt::NoModifier){
            switch(e->key()){
            case Qt::Key_F5:do_step();return;
            case Qt::Key_F6:do_run();return;
            case Qt::Key_F9:do_reset();return;
            case Qt::Key_F7:do_assemble();return;
            case Qt::Key_F8:do_assemble_and_run();return;
            }
        }
        QMainWindow::keyPressEvent(e);
    }
    void closeEvent(QCloseEvent* e) override {
        if (!current_file.isEmpty() && editor->document()->isModified()) {
            auto ret = QMessageBox::question(this, QString::fromUtf8("خروج"),
                QString::fromUtf8("الملف modified. حفظ قبل الخروج?"),
                QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            if (ret == QMessageBox::Save) { do_save(); e->accept(); }
            else if (ret == QMessageBox::Cancel) { e->ignore(); return; }
        }
        e->accept();
    }

private slots:
    void do_new_file(){
        if (!maybe_save()) return;
        editor->clear();current_file.clear();
        editor->document()->setModified(false);
        setWindowTitle(QString::fromUtf8("ض — Dhad IDE"));
        statusBar()->showMessage(QString::fromUtf8("ملف جديد"));
    }
    void do_open(){
        if (!maybe_save()) return;
        QString fn=QFileDialog::getOpenFileName(this,QString::fromUtf8("فتح ملف"),QDir::homePath(),
            "All (*)");
        if(fn.isEmpty())return;
        QFile f(fn);if(!f.open(QIODevice::ReadOnly|QIODevice::Text)){QMessageBox::warning(this,QString::fromUtf8("خطأ"),QString::fromUtf8("لا يمكن فتح الملف"));return;}
        editor->setPlainText(QString::fromUtf8(f.readAll()));
        current_file=fn;f.close();
        editor->document()->setModified(false);
        setWindowTitle(QString::fromUtf8("ض — %1").arg(fn));
        statusBar()->showMessage(QString::fromUtf8("تم الفتح: %1").arg(fn));
    }
    void do_save(){
        if(current_file.isEmpty()){do_save_as();return;}
        QFile f(current_file);if(!f.open(QIODevice::WriteOnly|QIODevice::Text)){QMessageBox::warning(this,QString::fromUtf8("خطأ"),QString::fromUtf8("لا يمكن الحفظ"));return;}
        f.write(editor->toPlainText().toUtf8());f.close();
        editor->document()->setModified(false);
        statusBar()->showMessage(QString::fromUtf8("تم الحفظ: %1").arg(current_file));
    }
    void do_save_as(){
        QString fn=QFileDialog::getSaveFileName(this,QString::fromUtf8("حفظ باسم"),QDir::homePath(),
            "All (*)");
        if(fn.isEmpty())return;
        current_file=fn;do_save();
        setWindowTitle(QString::fromUtf8("ض — %1").arg(fn));
    }
    bool maybe_save(){
        if(!editor->document()->isModified())return true;
        auto ret=QMessageBox::question(this,QString::fromUtf8("حفظ"),QString::fromUtf8("الملف modified. حفظ?"),
            QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel);
        if(ret==QMessageBox::Save){do_save();return true;}
        return ret==QMessageBox::Discard;
    }
    void do_assemble(){
        if(current_file.isEmpty()){
            statusBar()->showMessage(QString::fromUtf8("لا يوجد ملف مفتوح. احفظ أولاً (Ctrl+S)."),5000);return;
        }
        do_save();
        QByteArray fn_utf8=current_file.toUtf8();
        DhadAsm *a=new DhadAsm();
        memset(a,0,sizeof(DhadAsm));
        if(dhad_asm(a,fn_utf8.constData())<0){
            QString err=QString::fromUtf8(a->last_error);
            statusBar()->showMessage(err.isEmpty()?QString::fromUtf8("فشل التجميع"):QString::fromUtf8("خطأ: ")+err,10000);
            delete a;return;
        }
        CPU nc={};
        if(a->prog_size>0xF000){
            statusBar()->showMessage(QString::fromUtf8("الملف الثنائي كبير جداً"),5000);
            delete a;return;
        }
        nc.pc=PROG_START;
        nc.prog_size=(uint16_t)a->prog_size;
        memcpy(&nc.mem[PROG_START],a->program,a->prog_size);
        delete a;
        delete cpu;cpu=new CPU(nc);running=false;run_timer->stop();
        btn_run->setText(QString::fromUtf8("تشغيل"));btn_run->setStyleSheet(SHEET_BTN_SUCCESS);
        update_all();
        statusBar()->showMessage(QString::fromUtf8("تم التجميع: %1 (%2 بايت)").arg(current_file).arg(nc.prog_size),5000);
    }
    void do_assemble_and_run(){
        do_assemble();
        if(cpu && !cpu->halted) do_run();
    }
    void do_step(){if(!cpu||cpu->halted)return;cpu_step(*cpu);update_all();}
    void do_run(){
        if(!cpu){statusBar()->showMessage(QString::fromUtf8("لا يوجد برنامج محمل. اضغط تجميع أولاً (F7)."),5000);return;}
        running=!running;
        btn_run->setText(running?QString::fromUtf8("إيقاف"):QString::fromUtf8("تشغيل"));
        btn_run->setStyleSheet(running?SHEET_BTN_DANGER:SHEET_BTN_SUCCESS);
        running?run_timer->start():run_timer->stop();
    }
    void do_reset(){
        if(!cpu)return;
        CPU nc={};nc.pc=PROG_START;nc.stack_top=-1;
        nc.prog_size=cpu->prog_size;
        memcpy(&nc.mem[PROG_START],&cpu->mem[PROG_START],cpu->prog_size);
        delete cpu;cpu=new CPU(nc);running=false;run_timer->stop();
        btn_run->setText(QString::fromUtf8("تشغيل"));btn_run->setStyleSheet(SHEET_BTN_SUCCESS);
        update_all();
    }
    void do_speed(int s){run_timer->setInterval(max(1,500/s));}
    void timer_tick(){
        if(!cpu||cpu->halted){
            running=false;run_timer->stop();
            btn_run->setText(QString::fromUtf8("تشغيل"));btn_run->setStyleSheet(SHEET_BTN_SUCCESS);
            if(cpu)update_all();
            return;
        }
        if(running){
            int steps=max(1,speed_spin->value()*2);
            for(int i=0;i<steps&&!cpu->halted;i++) cpu_step(*cpu);
            update_all();
        }
    }
    void do_about(){
        QMessageBox::about(this,QString::fromUtf8("حول Dhad IDE"),
            QString::fromUtf8("<h2 style='color:#6B4226'>ض — Dhad IDE v4.0</h2>"
            "<p>محرر + مجمّع + محاكي + مصحح أخطاء</p>"
            "<p>معالج 8-بت مع 16 عنصر تعليمي</p>"
            "<p style='color:#8B6B4E'>Maghrib OS — 2026</p>"));
    }

private:
    CPU* cpu=nullptr;bool running=false;QTimer* run_timer=nullptr;
    QString current_file;
    QToolBar* tb=nullptr;
    CodeEditor* editor=nullptr;
    DhadHighlighter* highlighter=nullptr;
    QTableWidget* mem_table=nullptr;
    QTableWidget* inst_table=nullptr;
    RegisterBar* reg_bars[8];RegisterBar* acc_bar;
    CircularGauge* cycle_gauge;CircularGauge* inst_gauge;
    FlagIndicator* flg_z;FlagIndicator* flg_n;FlagIndicator* flg_c;
    StateBox* st_pc;StateBox* st_sp;StateBox* st_state;
    QLabel* lbl_output;
    QProgressBar* prog_bar;
    QPushButton* btn_run;
    QSpinBox* speed_spin;

    void setupMenus(){
        QMenu* fileMenu=menuBar()->addMenu(QString::fromUtf8("&ملف"));
        fileMenu->addAction(QString::fromUtf8("جديد (Ctrl+N)"),this,&MainWindow::do_new_file);
        fileMenu->addAction(QString::fromUtf8("فتح (Ctrl+O)"),this,&MainWindow::do_open);
        fileMenu->addAction(QString::fromUtf8("حفظ (Ctrl+S)"),this,&MainWindow::do_save);
        fileMenu->addAction(QString::fromUtf8("حفظ باسم... (Ctrl+Shift+S)"),this,&MainWindow::do_save_as);
        fileMenu->addSeparator();
        fileMenu->addAction(QString::fromUtf8("خروج"),this,&QMainWindow::close);

        QMenu* editMenu=menuBar()->addMenu(QString::fromUtf8("&تحرير"));
        editMenu->addAction(QString::fromUtf8("تراجع (Ctrl+Z)"),editor,&QPlainTextEdit::undo);
        editMenu->addAction(QString::fromUtf8("إعادة (Ctrl+Y)"),editor,&QPlainTextEdit::redo);
        editMenu->addSeparator();
        editMenu->addAction(QString::fromUtf8("قص (Ctrl+X)"),editor,&QPlainTextEdit::cut);
        editMenu->addAction(QString::fromUtf8("نسخ (Ctrl+C)"),editor,&QPlainTextEdit::copy);
        editMenu->addAction(QString::fromUtf8("لصق (Ctrl+V)"),editor,&QPlainTextEdit::paste);
        editMenu->addSeparator();
        editMenu->addAction(QString::fromUtf8("تحديد الكل (Ctrl+A)"),editor,&QPlainTextEdit::selectAll);
        editMenu->addAction(QString::fromUtf8("نقطة توقف (F9)"),this,[this](){
            editor->set_breakpoint(editor->textCursor().blockNumber()+1);
        });

        QMenu* runMenu=menuBar()->addMenu(QString::fromUtf8("&تشغيل"));
        runMenu->addAction(QString::fromUtf8("تجميع (F7)"),this,&MainWindow::do_assemble);
        runMenu->addAction(QString::fromUtf8("تجميع + تشغيل (F8)"),this,&MainWindow::do_assemble_and_run);
        runMenu->addSeparator();
        runMenu->addAction(QString::fromUtf8("خطوة (F5)"),this,&MainWindow::do_step);
        runMenu->addAction(QString::fromUtf8("تشغيل/إيقاف (F6)"),this,&MainWindow::do_run);
        runMenu->addAction(QString::fromUtf8("إعادة تعيين (F9)"),this,&MainWindow::do_reset);

        QMenu* helpMenu=menuBar()->addMenu(QString::fromUtf8("&مساعدة"));
        helpMenu->addAction(QString::fromUtf8("حول"),this,&MainWindow::do_about);

        menuBar()->setStyleSheet(QString(
            "QMenuBar{background:%1;color:%2;border-bottom:1px solid %3;font:11px 'Sans';}"
            "QMenuBar::item:selected{background:#D4C4B0;}"
            "QMenu{background:%1;color:%2;border:1px solid %3;}"
            "QMenu::item:selected{background:#D4C4B0;}"
        ).arg(CLR_BG_WARM, CLR_TEXT, CLR_BORDER));
    }

    QPushButton* make_btn(const QString& text,const QString& shortcut,const QString& style){
        QPushButton* b=new QPushButton(text+" ("+shortcut+")");
        b->setStyleSheet(style);b->setCursor(Qt::PointingHandCursor);return b;
    }

    void setup_ui(){
        QWidget* central=new QWidget(this);
        setCentralWidget(central);
        QVBoxLayout* main_lay=new QVBoxLayout(central);
        main_lay->setSpacing(1);main_lay->setContentsMargins(2,2,2,2);

        // شريط الأدوات
        tb=addToolBar("Tools");
        tb->setStyleSheet(QString(
            "QToolBar{background:%1;border-bottom:1px solid %2;spacing:4px;padding:5px;}"
        ).arg(CLR_BG_WARM, CLR_BORDER));
        tb->setMovable(false);

        tb->addWidget(make_btn(QString::fromUtf8("جديد"),"Ctrl+N",SHEET_BTN_NEUTRAL));
        connect(tb->children().last(),SIGNAL(clicked()),this,SLOT(do_new_file()));
        tb->addWidget(make_btn(QString::fromUtf8("فتح"),"Ctrl+O",SHEET_BTN_NEUTRAL));
        connect(tb->children().last(),SIGNAL(clicked()),this,SLOT(do_open()));
        tb->addWidget(make_btn(QString::fromUtf8("حفظ"),"Ctrl+S",SHEET_BTN_NEUTRAL));
        connect(tb->children().last(),SIGNAL(clicked()),this,SLOT(do_save()));
        tb->addSeparator();

        QPushButton* btn_asm=make_btn(QString::fromUtf8("تجميع"),"F7",SHEET_BTN_PRIMARY);
        connect(btn_asm,&QPushButton::clicked,this,&MainWindow::do_assemble);
        tb->addWidget(btn_asm);
        QPushButton* btn_asmrun=make_btn(QString::fromUtf8("تجميع+تشغيل"),"F8",SHEET_BTN_SUCCESS);
        connect(btn_asmrun,&QPushButton::clicked,this,&MainWindow::do_assemble_and_run);
        tb->addWidget(btn_asmrun);
        tb->addSeparator();

        QPushButton* tb_step=make_btn(QString::fromUtf8("خطوة"),"F5",SHEET_BTN_NEUTRAL);
        connect(tb_step,&QPushButton::clicked,this,&MainWindow::do_step);
        tb->addWidget(tb_step);
        btn_run=make_btn(QString::fromUtf8("تشغيل"),"F6",SHEET_BTN_SUCCESS);
        connect(btn_run,&QPushButton::clicked,this,&MainWindow::do_run);
        tb->addWidget(btn_run);
        QPushButton* tb_reset=make_btn(QString::fromUtf8("إعادة تعيين"),"F9",SHEET_BTN_DANGER);
        connect(tb_reset,&QPushButton::clicked,this,&MainWindow::do_reset);
        tb->addWidget(tb_reset);
        tb->addSeparator();

        QLabel* spd=new QLabel(QString::fromUtf8("السرعة:"));
        spd->setStyleSheet(QString("color:%1;font:bold 10px 'Sans';").arg(CLR_TEXT_SEC));
        tb->addWidget(spd);
        speed_spin=new QSpinBox();speed_spin->setRange(1,20);speed_spin->setValue(5);
        speed_spin->setStyleSheet(QString(
            "QSpinBox{background:%1;color:%2;border:1px solid %3;border-radius:4px;"
            "padding:3px 6px;font:bold 11px 'Monospace';}"
            "QSpinBox::up-button,QSpinBox::down-button{background:#EDE3D8;border:none;width:14px;}"
            "QSpinBox::up-button:hover,QSpinBox::down-button:hover{background:#D4C4B0;}"
        ).arg(CLR_BG_INPUT, CLR_TEXT, CLR_BORDER));
        connect(speed_spin,QOverload<int>::of(&QSpinBox::valueChanged),this,&MainWindow::do_speed);
        tb->addWidget(speed_spin);

        // المحتوى الرئيسي — علوي/سفلي
        QSplitter* main_split=new QSplitter(Qt::Vertical);
        main_split->setHandleWidth(2);
        main_split->setStyleSheet("QSplitter::handle{background:#C8B8A4;}");
        main_lay->addWidget(main_split,1);

        // علوي: محرر + سجلات
        QSplitter* top_split=new QSplitter(Qt::Horizontal);
        top_split->setHandleWidth(2);
        top_split->setStyleSheet("QSplitter::handle{background:#C8B8A4;}");
        main_split->addWidget(top_split);

        // محرر الكود
        QWidget* editor_wrap=new QWidget();
        QVBoxLayout* ed_wrap_lay=new QVBoxLayout(editor_wrap);
        ed_wrap_lay->setContentsMargins(0,0,0,0);
        editor=new CodeEditor();
        highlighter=new DhadHighlighter(editor->document());
        ed_wrap_lay->addWidget(editor);
        top_split->addWidget(editor_wrap);

        // السجلات والأعلام والحالة
        QWidget* right_panel=new QWidget();
        QVBoxLayout* right_lay=new QVBoxLayout(right_panel);
        right_lay->setContentsMargins(2,0,0,0);right_lay->setSpacing(2);

        QGroupBox* grp_reg=new QGroupBox(QString::fromUtf8("السجلات"));
        grp_reg->setStyleSheet(SHEET_GROUP);
        QWidget* reg_w=new QWidget();
        QGridLayout* reg_grid=new QGridLayout(reg_w);reg_grid->setSpacing(2);
        for(int i=0;i<8;i++){
            reg_bars[i]=new RegisterBar();
            reg_bars[i]->setLabel(QString("س%1").arg(i));
            reg_bars[i]->setColor(QColor(REG_COLORS[i]));
            reg_grid->addWidget(reg_bars[i],i/2,i%2);
        }
        acc_bar=new RegisterBar();acc_bar->setLabel(QString::fromUtf8("مح"));;acc_bar->setColor(QColor("#B85C38"));
        reg_grid->addWidget(acc_bar,4,0,1,2);
        QGridLayout* g1=new QGridLayout(grp_reg);g1->addWidget(reg_w);
        right_lay->addWidget(grp_reg);

        QWidget* flags_state=new QWidget();
        QHBoxLayout* fs_lay=new QHBoxLayout(flags_state);fs_lay->setContentsMargins(0,0,0,0);fs_lay->setSpacing(2);
        QGroupBox* grp_flg=new QGroupBox(QString::fromUtf8("الأعلام"));
        grp_flg->setStyleSheet(SHEET_GROUP);
        QWidget* flg_w=new QWidget();
        QHBoxLayout* flg_lay=new QHBoxLayout(flg_w);
        flg_z=new FlagIndicator("Z",QColor("#5A7247"));
        flg_n=new FlagIndicator("N",QColor("#B85C38"));
        flg_c=new FlagIndicator("C",QColor("#9B7340"));
        flg_lay->addWidget(flg_z);flg_lay->addWidget(flg_n);flg_lay->addWidget(flg_c);
        QGridLayout* g2=new QGridLayout(grp_flg);g2->addWidget(flg_w);
        fs_lay->addWidget(grp_flg);

        QGroupBox* grp_st=new QGroupBox(QString::fromUtf8("الحالة"));
        grp_st->setStyleSheet(SHEET_GROUP);
        QWidget* st_w=new QWidget();
        QGridLayout* st_grid=new QGridLayout(st_w);
        st_pc=new StateBox(QString::fromUtf8("البرنامج"));;st_sp=new StateBox(QString::fromUtf8("المكدس"));;st_state=new StateBox(QString::fromUtf8("الحالة"));
        st_grid->addWidget(st_pc,0,0);st_grid->addWidget(st_sp,0,1);
        st_grid->addWidget(st_state,1,0,1,2);
        QGridLayout* g3=new QGridLayout(grp_st);g3->addWidget(st_w);
        fs_lay->addWidget(grp_st);
        right_lay->addWidget(flags_state);

        QGroupBox* grp_perf=new QGroupBox(QString::fromUtf8("الأداء"));
        grp_perf->setStyleSheet(SHEET_GROUP);
        QWidget* perf_w=new QWidget();
        QHBoxLayout* perf_lay=new QHBoxLayout(perf_w);
        cycle_gauge=new CircularGauge();cycle_gauge->setLabel(QString::fromUtf8("دورات"));cycle_gauge->setMax(100000);
        inst_gauge=new CircularGauge();inst_gauge->setLabel(QString::fromUtf8("تعليمات"));inst_gauge->setMax(50000);
        inst_gauge->setColor(QColor("#5A7247"));
        perf_lay->addWidget(cycle_gauge);perf_lay->addWidget(inst_gauge);
        QGridLayout* g4=new QGridLayout(grp_perf);g4->addWidget(perf_w);
        right_lay->addWidget(grp_perf);

        top_split->addWidget(right_panel);

        // سفلي: تعليمات + ذاكرة + إخراج
        QTabWidget* bottom_tabs=new QTabWidget();
        bottom_tabs->setStyleSheet(QString(
            "QTabWidget::pane{border:1px solid %1;background:%2;}"
            "QTabBar::tab{background:%3;color:%4;padding:5px 14px;border:1px solid %1;"
            "border-bottom:none;border-top-left-radius:5px;border-top-right-radius:5px;"
            "margin-right:2px;font:10px 'Sans';}"
            "QTabBar::tab:selected{background:%2;color:%5;border-bottom:2px solid %5;}"
            "QTabBar::tab:hover{background:%6;}"
        ).arg(CLR_BORDER, CLR_BG, CLR_BG_CARD, CLR_TEXT_DIM, CLR_WOOD, CLR_BG_HOVER));
        bottom_tabs->setMinimumHeight(150);

        QWidget* disasm_tab=new QWidget();
        QVBoxLayout* dis_lay=new QVBoxLayout(disasm_tab);dis_lay->setContentsMargins(0,0,0,0);
        inst_table=new QTableWidget(0,4);
        inst_table->setHorizontalHeaderLabels({QString::fromUtf8(""),QString::fromUtf8("العنوان"),QString::fromUtf8("سداسي"),QString::fromUtf8("التعليمية")});
        inst_table->setStyleSheet(SHEET_TABLE);
        inst_table->verticalHeader()->setVisible(false);
        inst_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        inst_table->setAlternatingRowColors(true);
        inst_table->verticalHeader()->setDefaultSectionSize(18);
        dis_lay->addWidget(inst_table);
        bottom_tabs->addTab(disasm_tab,QString::fromUtf8("التعليمات"));

        QWidget* mem_tab=new QWidget();
        QVBoxLayout* mem_lay=new QVBoxLayout(mem_tab);mem_lay->setContentsMargins(0,0,0,0);
        mem_table=new QTableWidget(0,4);
        mem_table->setHorizontalHeaderLabels({QString::fromUtf8("العنوان"),QString::fromUtf8("سداسي"),QString::fromUtf8("حرف"),QString::fromUtf8("التعليمية")});
        mem_table->setStyleSheet(SHEET_TABLE);
        mem_table->verticalHeader()->setVisible(false);
        mem_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        mem_table->setAlternatingRowColors(true);
        mem_table->verticalHeader()->setDefaultSectionSize(18);
        mem_lay->addWidget(mem_table);
        bottom_tabs->addTab(mem_tab,QString::fromUtf8("الذاكرة"));

        QWidget* out_tab=new QWidget();
        QVBoxLayout* out_lay=new QVBoxLayout(out_tab);out_lay->setContentsMargins(0,0,0,0);
        lbl_output=new QLabel("");
        lbl_output->setWordWrap(true);
        lbl_output->setStyleSheet(QString(
            "QLabel{background:%1;color:%2;font:15px 'Monospace';border:1px solid %3;border-radius:4px;padding:8px;}"
        ).arg(CLR_BG_INPUT, CLR_WOOD, CLR_BORDER));
        lbl_output->setAlignment(Qt::AlignTop|Qt::AlignLeft);
        QScrollArea* out_scroll=new QScrollArea();
        out_scroll->setWidget(lbl_output);out_scroll->setWidgetResizable(true);
        out_scroll->setStyleSheet(QString("QScrollArea{background:%1;border:none;}").arg(CLR_BG));
        out_lay->addWidget(out_scroll);
        bottom_tabs->addTab(out_tab,QString::fromUtf8("الإخراج"));

        main_split->addWidget(bottom_tabs);

        // شريط التقدم
        prog_bar=new QProgressBar();
        prog_bar->setRange(0,100);prog_bar->setValue(0);
        prog_bar->setTextVisible(true);        prog_bar->setFormat(QString::fromUtf8("البرنامج: %v%"));
        prog_bar->setStyleSheet(QString(
            "QProgressBar{border:1px solid %1;border-radius:4px;text-align:center;"
            "color:%2;font:bold 9px 'Sans';background:%3;}"
            "QProgressBar::chunk{background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
            "stop:0 %4,stop:1 %5);border-radius:3px;}"
        ).arg(CLR_BORDER, CLR_TEXT, CLR_BG_CARD, CLR_WOOD, CLR_ACTIVE));
        main_lay->addWidget(prog_bar);

        // المؤقت
        run_timer=new QTimer(this);run_timer->setInterval(100);
        connect(run_timer,&QTimer::timeout,this,&MainWindow::timer_tick);

        // شريط الحالة
        statusBar()->setStyleSheet(QString(
            "QStatusBar{background:%1;color:%2;border-top:1px solid %3;font:10px 'Sans';}"
        ).arg(CLR_BG_WARM, CLR_TEXT_SEC, CLR_BORDER));
        statusBar()->showMessage(QString::fromUtf8("جاهز | F5:خطوة  F6:تشغيل  F7:تجميع  F8:تجميع+تشغيل  F9:إعادة تعيين"));
    }

    void update_all(){
        if(!cpu)return;
        update_registers();update_flags();update_state();
        update_memory();update_instructions();update_output();update_progress();
        cycle_gauge->setTarget((int)cpu->cycle);
        inst_gauge->setTarget((int)cpu->inst_count);
    }

    void update_registers(){
        for(int i=0;i<8;i++)reg_bars[i]->setValue(cpu->regs[i]);
        acc_bar->setValue(cpu->acc);
    }

    void update_flags(){
        flg_z->setOn(cpu->flags&1);
        flg_n->setOn(cpu->flags&2);
        flg_c->setOn(cpu->flags&4);
    }

    void update_state(){
        st_pc->setValue(QString("0x%1").arg(cpu->pc,4,16,QChar('0')));
        st_sp->setValue(QString::number(cpu->sp));
        st_state->setValue(cpu->halted?QString::fromUtf8("متوقف"):QString::fromUtf8("يعمل"));
        st_state->setValColor(cpu->halted?CLR_DANGER:CLR_ACTIVE);
    }

    void update_memory(){
        mem_table->setRowCount(0);
        uint16_t start=(cpu->pc>=8)?cpu->pc-8:PROG_START;
        for(int i=0;i<16&&(start+i)<MEM_SIZE;i++){
            uint16_t addr=start+i;uint8_t val=cpu->mem[addr];
            bool cur=(addr==cpu->pc);
            int row=mem_table->rowCount();mem_table->insertRow(row);
            auto mk=[](const QString& t,bool bold=false){
                QTableWidgetItem* it=new QTableWidgetItem(t);
                it->setTextAlignment(Qt::AlignCenter);
                it->setFont(QFont("Monospace",10,bold?QFont::Bold:QFont::Normal));
                return it;
            };
            mem_table->setItem(row,0,mk(QString("0x%1").arg(addr,4,16,QChar('0'))));
            mem_table->setItem(row,1,mk(QString("%1").arg(val,2,16,QChar('0')).toUpper(),true));
            mem_table->setItem(row,2,mk((val>=32&&val<127)?QString(QChar(val)):"."));
            uint8_t opc=(val>>4)&0xF;
            QString inst;
            if(cur){
                if(opc==0xF){uint8_t ex=(addr+1<MEM_SIZE)?cpu->mem[addr+1]:0;inst=QString(">> %1").arg(op_name(ex&0xF0));}
                else inst=QString(">> %1").arg(op_name(opc));
            }
            mem_table->setItem(row,3,mk(inst,true));
            if(cur)for(int c=0;c<4;c++)mem_table->item(row,c)->setBackground(QColor(212,196,176,120));
        }
    }

    void update_instructions(){
        inst_table->setRowCount(0);
        uint16_t start=(cpu->pc>=6)?cpu->pc-6:PROG_START;
        for(int i=0;i<14&&(start+i)<MEM_SIZE;i++){
            uint16_t addr=start+i;uint8_t val=cpu->mem[addr];
            uint8_t opc=(val>>4)&0xF;
            bool cur=(addr==cpu->pc);
            int row=inst_table->rowCount();inst_table->insertRow(row);
            auto mk=[](const QString& t,bool bold=false,const QColor& fg=QColor(CLR_TEXT)){
                QTableWidgetItem* it=new QTableWidgetItem(t);
                it->setTextAlignment(Qt::AlignCenter);
                it->setFont(QFont("Monospace",10,bold?QFont::Bold:QFont::Normal));
                it->setForeground(fg);return it;
            };
            inst_table->setItem(row,0,mk(cur?">>":"",true,QColor(CLR_ACTIVE)));
            inst_table->setItem(row,1,mk(QString("0x%1").arg(addr,4,16,QChar('0'))));
            inst_table->setItem(row,2,mk(QString("%1").arg(val,2,16,QChar('0')).toUpper(),true));
            QString inst;
            if(opc==0xF){
                uint8_t ex=(addr+1<MEM_SIZE)?cpu->mem[addr+1]:0;
                inst=op_name(ex&0xF0);
                if((ex&0xF0)<=0x40){inst+=QString(" 0x%1").arg(fetch16_addr(addr),4,16,QChar('0'));}
                else{int r=ex&0xF;if(r<8)inst+=QString(" R%1").arg(r);}
            }else{
                inst=op_name(opc);
                if(opc==OP_LOAD)inst+=QString(" %1").arg(val&0xF);
                else if(opc>=OP_ADD&&opc<=OP_MOD)inst+=QString(" R%1").arg(val&0xF);
                else if(opc==OP_CALL){inst+=QString(" 0x%1").arg(fetch16_addr(addr),4,16,QChar('0'));}
            }
            inst_table->setItem(row,3,mk(inst,false,cur?QColor(CLR_WOOD):QColor(CLR_TEXT)));
            if(cur)for(int c=0;c<4;c++)inst_table->item(row,c)->setBackground(QColor(212,196,176,120));
        }
    }

    uint16_t fetch16_addr(uint16_t addr){
        uint8_t lo=(addr+1<MEM_SIZE)?cpu->mem[addr+1]:0;
        uint8_t hi=(addr+2<MEM_SIZE)?cpu->mem[addr+2]:0;
        return (hi<<8)|lo;
    }

    void update_output(){lbl_output->setText(QString::fromStdString(cpu->output));}

    void update_progress(){
        if(cpu->prog_size>0){
            int pct=(int)((cpu->pc-PROG_START)*100/cpu->prog_size);
            prog_bar->setValue(qBound(0,pct,100));
        }
    }
};

/* ═══════════════════════════════════════════════════════════════════════════════
 *  main
 * ═══════════════════════════════════════════════════════════════════════════════ */

int main(int argc,char* argv[]){
    signal(SIGSEGV, crash_handler);
    signal(SIGABRT, crash_handler);
    QApplication app(argc,argv);
    app.setApplicationName("Dhad IDE");
    app.setApplicationVersion("4.0");
    app.setStyle("Fusion");
    QPalette light;
    light.setColor(QPalette::Window,QColor("#FFFBF7"));
    light.setColor(QPalette::WindowText,QColor("#2C1810"));
    light.setColor(QPalette::Base,QColor("#FFFBF7"));
    light.setColor(QPalette::AlternateBase,QColor("#F5EDE4"));
    light.setColor(QPalette::Text,QColor("#2C1810"));
    light.setColor(QPalette::Button,QColor("#EDE3D8"));
    light.setColor(QPalette::ButtonText,QColor("#2C1810"));
    light.setColor(QPalette::Highlight,QColor("#D4C4B0"));
    light.setColor(QPalette::HighlightedText,QColor("#2C1810"));
    light.setColor(QPalette::PlaceholderText,QColor("#A08A70"));
    app.setPalette(light);
    app.setFont(QFont("Sans",10));
    MainWindow w;w.show();
    return app.exec();
}

#include "main.moc"
