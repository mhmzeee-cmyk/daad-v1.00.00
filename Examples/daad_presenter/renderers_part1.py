# -*- coding: utf-8 -*-
"""
================================================================================
DAAD PRESENTATION ENGINE - CUSTOM RENDERING SERVICES (SLIDES 1-10)
================================================================================
Highly comprehensive, custom slide-by-slide layout system.
================================================================================
"""

from pptx.util import Inches, Pt
from pptx.enum.shapes import MSO_SHAPE
from pptx.enum.text import PP_ALIGN
from daad_presenter.theme import ThemeColors, Typography, Dimensions
from daad_presenter.shapes import ShapeService

class RenderersPart1:
    @staticmethod
    def render_cover(slide, sd, slide_idx):
        """Slide 01: Professional Cover with bottom geometric bar."""
        # Top Accent Line
        accent = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, Inches(0), Inches(0), Dimensions.SCREEN_W, Inches(0.08))
        accent.fill.solid()
        accent.fill.fore_color.rgb = ThemeColors.PRIMARY
        accent.line.fill.background()

        # Decorative geometric cluster (top-right)
        ShapeService.draw_icon_badge(slide, Inches(11.4), Inches(0.55), Inches(0.9), inner="diamond", fill_color=ThemeColors.BG_CARD, border_color=ThemeColors.PRIMARY)
        ShapeService.draw_diamond(slide, Inches(12.5), Inches(0.75), Inches(0.35), ThemeColors.ACCENT)
        circ_big = slide.shapes.add_shape(MSO_SHAPE.OVAL, Inches(12.15), Inches(0.2), Inches(0.5), Inches(0.5))
        circ_big.fill.background()
        circ_big.line.color.rgb = ThemeColors.ACCENT
        circ_big.line.width = Pt(1)

        # Decorative floating plus signs (left side)
        ShapeService.draw_plus(slide, Inches(0.9), Inches(1.4), Inches(0.35), ThemeColors.ACCENT)
        ShapeService.draw_plus(slide, Inches(0.5), Inches(2.6), Inches(0.22), ThemeColors.PRIMARY)

        # Title Text Block
        ShapeService.draw_textbox(slide, Inches(1), Inches(2.2), Inches(11.333), Inches(1.5), sd.get("title", ""), font_size=72, font_color=ThemeColors.PRIMARY, bold=True, align=PP_ALIGN.CENTER)
        
        # Subtitle Text Block
        ShapeService.draw_textbox(slide, Inches(1), Inches(3.7), Inches(11.333), Inches(0.6), sd.get("subtitle", ""), font_size=26, font_color=ThemeColors.TEXT_DARK, align=PP_ALIGN.CENTER)
        
        # Divider Line
        div = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, Inches(5.5), Inches(4.5), Inches(2.333), Inches(0.02))
        div.fill.solid()
        div.fill.fore_color.rgb = ThemeColors.ACCENT
        div.line.fill.background()
        
        # Meta Info Lines
        lines = sd.get("lines", [])
        for i, line in enumerate(lines):
            color = ThemeColors.TEXT_MUTED if i == 0 else ThemeColors.TEXT_BODY
            ShapeService.draw_textbox(slide, Inches(1), Inches(4.9 + i*0.55), Inches(11.333), Inches(0.4), line.get("text", ""), font_size=12, font_color=color, align=PP_ALIGN.CENTER)

        # Bottom geometric decorations
        for i in range(5):
            bar = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, Inches(0.5 + i*0.55), Inches(6.6), Inches(0.35), Inches(0.35))
            bar.fill.solid()
            bar.fill.fore_color.rgb = ThemeColors.ACCENT if i % 2 == 0 else ThemeColors.PRIMARY
            bar.line.fill.background()
            bar.rotation = 45
        ShapeService.draw_icon_badge(slide, Inches(3.6), Inches(6.55), Inches(0.45), inner="square", fill_color=ThemeColors.WHITE, border_color=ThemeColors.PRIMARY)
        ShapeService.draw_icon_badge(slide, Inches(9.3), Inches(6.55), Inches(0.45), inner="plus", fill_color=ThemeColors.WHITE, border_color=ThemeColors.SECONDARY)
        ShapeService.draw_plus(slide, Inches(11.0), Inches(6.6), Inches(0.3), ThemeColors.ACCENT)

    @staticmethod
    def render_slide_2_what_is_daad(slide, sd, slide_idx):
        """Slide 02: Matches Target Image 01 (Identity, Goals, and Journey)."""
        ShapeService.draw_header(slide, "من هي لغة ض؟", slide_idx)
        
        # Left Side Column (Description + Target Goal Box)
        ShapeService.draw_textbox(slide, Inches(0.5), Inches(1.3), Inches(4.0), Inches(1.8), 
                                  "«ض» هي لغة برمجة عربية عالية المستوى، تم تصميمها لتكون بسيطة، واضحة، وقريبة من طريقة تفكير المبرمج العربي.", 
                                  font_size=15, font_color=ThemeColors.TEXT_BODY, align=PP_ALIGN.RIGHT)
        
        # Goal Box (Mint Green background as shown in Image 01)
        goal_y = Inches(3.2)
        ShapeService.draw_card(slide, Inches(0.5), goal_y, Inches(4.0), Inches(2.1), ThemeColors.BG_CARD, ThemeColors.PRIMARY)
        # Target icon drawn using concentric vector circles
        outer_circle = slide.shapes.add_shape(MSO_SHAPE.OVAL, Inches(0.7), goal_y + Inches(0.1), Inches(0.5), Inches(0.5))
        outer_circle.fill.solid(); outer_circle.fill.fore_color.rgb = ThemeColors.WHITE; outer_circle.line.color.rgb = ThemeColors.PRIMARY
        inner_circle = slide.shapes.add_shape(MSO_SHAPE.OVAL, Inches(0.85), goal_y + Inches(0.25), Inches(0.2), Inches(0.2))
        inner_circle.fill.solid(); inner_circle.fill.fore_color.rgb = ThemeColors.PRIMARY; inner_circle.line.fill.background()
        
        ShapeService.draw_textbox(slide, Inches(1.3), goal_y + Inches(0.1), Inches(3.0), Inches(0.4), "هدفنا", font_size=17, font_color=ThemeColors.PRIMARY, bold=True, align=PP_ALIGN.RIGHT)
        ShapeService.draw_textbox(slide, Inches(0.7), goal_y + Inches(0.7), Inches(3.6), Inches(1.3), 
                                  "تمكين المبرمج العربي من كتابة البرامج بالعربية بأسلوب سهل ومنطقي، مع بناء مترجم قوي وقابل للتطور حتى الوصول إلى لغة متكاملة تدعم المعالجات.", 
                                  font_size=13, font_color=ThemeColors.TEXT_BODY, align=PP_ALIGN.RIGHT)
                                
        # Target Audience Box (Bottom Left - "لمن صممت؟")
        aud_y = Inches(5.4)
        ShapeService.draw_card(slide, Inches(0.5), aud_y, Inches(4.0), Inches(1.7), ThemeColors.WHITE, ThemeColors.BORDER, 0.5)
        ShapeService.draw_textbox(slide, Inches(0.7), aud_y + Inches(0.1), Inches(3.6), Inches(0.35), "لمن صممت؟", font_size=15, font_color=ThemeColors.SECONDARY, bold=True, align=PP_ALIGN.RIGHT)
        audiences = ["الطلاب والمتعلمين لدعمهم في التعليم البرمجي بوضوح", "المعلمون والمؤسسات لتوفير نظام تعليمي فعال", "المطورون العرب لبناء كود عربي أصيل وقوي"]
        ShapeService.draw_bullet_list(slide, Inches(0.7), aud_y + Inches(0.45), Inches(3.6), Inches(1.15), audiences, font_size=12)

        # Right Side Column (Pillars & Flowchart)
        ShapeService.draw_textbox(slide, Inches(4.8), Inches(1.15), Inches(8.0), Inches(0.4), "ما يميز لغة «ض»", font_size=18, font_color=ThemeColors.SECONDARY, bold=True, align=PP_ALIGN.RIGHT)
        
        # 5 Vertical Feature Cards (Image 01)
        features = [
            ("مستقبل متكامل", "رؤية واضحة للوصول إلى مستوى المعالجات."),
            ("مترجم متطور", "بنية مترجم احترافية ومفتوحة المصدر."),
            ("قابلة للتوسع", "مصممة للنمو والتطوير المستمر."),
            ("بسيطة وسهلة", "قواعد مرنة تسرع من عملية التعلم."),
            ("عربية طبيعية", "كتابة الأكواد بكلمات مألوفة تماماً.")
        ]
        
        for i, (title, desc) in enumerate(features):
            card_x = Inches(4.8 + i*1.6)
            card_y = Inches(1.6)
            title_color = ThemeColors.cycle(i)
            ShapeService.draw_card(slide, card_x, card_y, Inches(1.5), Inches(3.2), ThemeColors.WHITE, ThemeColors.BORDER, 0.5)
            # Custom geometric placeholder shape for icon
            icon_circle = slide.shapes.add_shape(MSO_SHAPE.OVAL, card_x + Inches(0.4), card_y + Inches(0.2), Inches(0.7), Inches(0.7))
            icon_circle.fill.solid(); icon_circle.fill.fore_color.rgb = ThemeColors.BG_CARD; icon_circle.line.color.rgb = title_color
            ShapeService.draw_diamond(slide, card_x + Inches(0.64), card_y + Inches(0.44), Inches(0.22), title_color)
            
            ShapeService.draw_textbox(slide, card_x, card_y + Inches(1.0), Inches(1.5), Inches(0.5), title, font_size=13, font_color=title_color, bold=True, align=PP_ALIGN.CENTER)
            ShapeService.draw_textbox(slide, card_x + Inches(0.08), card_y + Inches(1.6), Inches(1.34), Inches(1.5), desc, font_size=10, font_color=ThemeColors.TEXT_BODY, align=PP_ALIGN.CENTER)

        # Bottom Flowchart Card: "رحلة برنامج ض" (Image 01)
        flow_y = Inches(5.0)
        ShapeService.draw_card(slide, Inches(4.8), flow_y, Inches(8.0), Inches(2.1), ThemeColors.BG_CARD, ThemeColors.PRIMARY)
        ShapeService.draw_textbox(slide, Inches(5.0), flow_y + Inches(0.1), Inches(7.6), Inches(0.35), "رحلة برنامج «ض» من الكتابة إلى التنفيذ", font_size=13, font_color=ThemeColors.PRIMARY, bold=True, align=PP_ALIGN.CENTER)
        
        steps = ["ملف مصدر", "Lexer", "Parser", "Semantic", "Backend", "الناتج"]
        for i, step in enumerate(steps):
            step_x = Inches(5.1 + i*1.28)
            # Flow Circle
            circ = slide.shapes.add_shape(MSO_SHAPE.OVAL, step_x + Inches(0.34), flow_y + Inches(0.55), Inches(0.6), Inches(0.6))
            circ.fill.solid(); circ.fill.fore_color.rgb = ThemeColors.WHITE; circ.line.color.rgb = ThemeColors.PRIMARY
            ShapeService.draw_textbox(slide, step_x, flow_y + Inches(1.2), Inches(1.28), Inches(0.4), step, font_size=10, font_color=ThemeColors.TEXT_DARK, bold=True, align=PP_ALIGN.CENTER)
            
            # Flow Arrow
            if i < len(steps) - 1:
                ShapeService.draw_arrow(slide, step_x + Inches(1.05), flow_y + Inches(0.72), Inches(0.18), Inches(0.18))

    @staticmethod
    def render_slide_3_core_components(slide, sd, slide_idx):
        """Slide 03: Matches Target Image 03 (Language Pillars, Code Block, Table)."""
        ShapeService.draw_header(slide, "مكونات اللغة الأساسية", slide_idx)
        
        # Sub-header Intro
        ShapeService.draw_textbox(slide, Inches(0.5), Inches(1.05), Inches(12.333), Inches(0.4), "توفر «ض» مجموعة متكاملة من المكونات لبناء البرامج بسهولة ووضوح.", font_size=16, font_color=ThemeColors.TEXT_BODY, align=PP_ALIGN.CENTER)
        
        # LEFT COLUMN (5 Vertical Cards)
        pillars = [
            ("المتغيرات والثوابت", "تعريف وتخزين البيانات والتحكم في القيم."),
            ("أنواع البيانات", "أنواع متعددة كالأعداد والنصوص لتناسب احتياجك."),
            ("العمليات والتعبيرات", "عمليات حسابية ومنطقية لبناء تعبيرات مرنة."),
            ("الشروط والحلقات", "تنفيذ قرارات وتكرار الأوامر بالشرط."),
            ("الدوال والإجراءات", "تنظيم الكود في دوال قابلة لإعادة الاستخدام.")
        ]
        
        for i, (title, desc) in enumerate(pillars):
            card_y = Inches(1.5 + i*1.15)
            ShapeService.draw_card(slide, Inches(0.5), card_y, Inches(3.2), Inches(1.05), ThemeColors.WHITE, ThemeColors.BORDER, 0.5)
            # Draw green tag on left
            tag = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, Inches(0.5), card_y, Inches(0.06), Inches(1.05))
            tag.fill.solid(); tag.fill.fore_color.rgb = ThemeColors.cycle(i); tag.line.fill.background()
            
            ShapeService.draw_textbox(slide, Inches(0.65), card_y + Inches(0.08), Inches(2.9), Inches(0.35), title, font_size=14, font_color=ThemeColors.cycle(i), bold=True, align=PP_ALIGN.RIGHT)
            ShapeService.draw_textbox(slide, Inches(0.65), card_y + Inches(0.45), Inches(2.9), Inches(0.55), desc, font_size=11, font_color=ThemeColors.TEXT_MUTED, align=PP_ALIGN.RIGHT)

        # MIDDLE COLUMN (Code Block)
        code_lines = [
            "صحيح س = 5 ؛",
            "صحيح مجموع = 0 ؛",
            "طالما ( س > 0 ) {",
            "    مجموع = مجموع + س ؛",
            "    س = س - 1 ؛",
            "}",
            "اطبع( \"المجموع النهائي: \", مجموع ) ؛"
        ]
        ShapeService.draw_ide_block(slide, Inches(3.9), Inches(1.5), Inches(4.8), Inches(5.6), "مثال برنامج بلغة ض", "\n".join(code_lines))

        # RIGHT COLUMN (Data Types Table)
        tbl_x = Inches(8.9)
        tbl_y = Inches(1.5)
        tbl_w = Inches(3.933)
        tbl_h = Inches(5.6)
        
        # Table Container
        ShapeService.draw_card(slide, tbl_x, tbl_y, tbl_w, tbl_h, ThemeColors.WHITE, ThemeColors.BORDER, 0.5)
        # Header Row
        tb_head = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, tbl_x, tbl_y, tbl_w, Inches(0.45))
        tb_head.fill.solid(); tb_head.fill.fore_color.rgb = ThemeColors.PRIMARY; tb_head.line.fill.background()
        ShapeService.draw_textbox(slide, tbl_x, tbl_y, tbl_w, Inches(0.45), "أمثلة أنواع البيانات", font_size=14, font_color=ThemeColors.WHITE, bold=True, align=PP_ALIGN.CENTER)
        
        # Grid rows manually computed to guarantee absolute alignment
        rows_data = [
            ("عدد", "يمثل الأعداد الصحيحة والعشرية", "10 / 3.14"),
            ("نص", "يمثل النصوص السلاسل الحرفية", "\"مرحبا\""),
            ("منطقي", "يمثل القيم المنطقية (صحيح/خطأ)", "صحيح / خطأ"),
            ("حرف", "يمثل حرفاً واحداً", "'ض'"),
            ("مصفوفة", "مجموعة من القيم من نفس النوع", "[1, 2, 3]"),
            ("دالة", "كتلة برمجية تؤدي مهمة محددة", "الدالة احسب()")
        ]
        
        for idx, (t, d, ex) in enumerate(rows_data):
            row_y = tbl_y + Inches(0.45 + idx * 0.85)
            # Alternating rows
            bg_col = ThemeColors.BG_CARD if idx % 2 == 0 else ThemeColors.WHITE
            row_bg = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, tbl_x + Inches(0.02), row_y, tbl_w - Inches(0.04), Inches(0.8))
            row_bg.fill.solid(); row_bg.fill.fore_color.rgb = bg_col; row_bg.line.fill.background()
            
            # Sub-columns
            ShapeService.draw_textbox(slide, tbl_x + Inches(2.8), row_y + Inches(0.1), Inches(1.0), Inches(0.6), t, font_size=13, font_color=ThemeColors.cycle(idx), bold=True, align=PP_ALIGN.CENTER)
            ShapeService.draw_textbox(slide, tbl_x + Inches(1.2), row_y + Inches(0.1), Inches(1.5), Inches(0.6), d, font_size=11, font_color=ThemeColors.TEXT_BODY, align=PP_ALIGN.CENTER)
            ShapeService.draw_textbox(slide, tbl_x + Inches(0.1), row_y + Inches(0.1), Inches(1.0), Inches(0.6), ex, font_size=12, font_color=ThemeColors.SECONDARY, font_name=Typography.CODE, align=PP_ALIGN.CENTER)

    @staticmethod
    def render_slide_4_audience(slide, sd, slide_idx):
        """Slide 04: Beautiful 4-column customized layout for Audience."""
        ShapeService.draw_header(slide, "لمن صممت لغة ض؟", slide_idx)
        audiences = sd.get("items", [])
        col_w = Inches(2.8)
        gap = Inches(0.2)
        start_x = Inches(0.5)
        
        for i, item in enumerate(audiences[:4]):
            x = start_x + i * (col_w + gap)
            y = Inches(1.8)
            title_color = ThemeColors.cycle(i)
            ShapeService.draw_card(slide, x, y, col_w, Inches(4.8), ThemeColors.WHITE, ThemeColors.PRIMARY, 1)
            
            # Circular icon placeholder
            circ = slide.shapes.add_shape(MSO_SHAPE.OVAL, x + Inches(0.9), y + Inches(0.4), Inches(1.0), Inches(1.0))
            circ.fill.solid(); circ.fill.fore_color.rgb = ThemeColors.BG_CARD; circ.line.color.rgb = title_color
            ShapeService.draw_textbox(slide, x + Inches(0.9), y + Inches(0.65), Inches(1.0), Inches(0.5), f"0{i+1}", font_size=22, font_color=title_color, bold=True, align=PP_ALIGN.CENTER)
            
            # Small inner geometric icon inside the circle
            ShapeService.draw_diamond(slide, x + Inches(1.37), y + Inches(0.87), Inches(0.16), title_color)
            
            # Corner badge on the card
            ShapeService.draw_icon_badge(slide, x + col_w - Inches(0.55), y + Inches(0.15), Inches(0.4), inner="dot", fill_color=ThemeColors.WHITE, border_color=title_color)
            
            # Text content
            ShapeService.draw_textbox(slide, x + Inches(0.2), y + Inches(1.8), col_w - Inches(0.4), Inches(0.6), item.get("t", ""), font_size=19, font_color=title_color, bold=True, align=PP_ALIGN.CENTER)
            ShapeService.draw_textbox(slide, x + Inches(0.2), y + Inches(2.6), col_w - Inches(0.4), Inches(2.0), item.get("d", ""), font_size=13, font_color=ThemeColors.TEXT_BODY, align=PP_ALIGN.CENTER)

    @staticmethod
    def render_slide_5_philosophy(slide, sd, slide_idx):
        """Slide 05: Design Philosophy - horizontal layout with top-anchored stripes."""
        ShapeService.draw_header(slide, "فلسفة تصميم لغة ض", slide_idx)
        principles = sd.get("items", [])
        col_w = Inches(2.8)
        gap = Inches(0.2)
        start_x = Inches(0.5)
        
        for i, item in enumerate(principles[:4]):
            x = start_x + i * (col_w + gap)
            y = Inches(2.0)
            title_color = ThemeColors.cycle(i)
            ShapeService.draw_card(slide, x, y, col_w, Inches(4.5), ThemeColors.WHITE, ThemeColors.BORDER, 0.5)
            
            # Thick top green bar
            bar = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, x, y, col_w, Inches(0.25))
            bar.fill.solid(); bar.fill.fore_color.rgb = title_color; bar.line.fill.background()
            
            # Number badge on the bar
            ShapeService.draw_icon_badge(slide, x + Inches(0.15), y + Inches(0.15), Inches(0.4), inner="square", fill_color=ThemeColors.WHITE, border_color=title_color)
            ShapeService.draw_diamond(slide, x + col_w - Inches(0.5), y + Inches(0.07), Inches(0.14), title_color)
            
            ShapeService.draw_textbox(slide, x + Inches(0.2), y + Inches(0.6), col_w - Inches(0.4), Inches(0.6), item.get("t", ""), font_size=18, font_color=title_color, bold=True, align=PP_ALIGN.CENTER)
            ShapeService.draw_textbox(slide, x + Inches(0.2), y + Inches(1.5), col_w - Inches(0.4), Inches(2.5), item.get("d", ""), font_size=13, font_color=ThemeColors.TEXT_BODY, align=PP_ALIGN.CENTER)

    @staticmethod
    def render_slide_6_split_screen(slide, sd, slide_idx):
        """Slide 06: Beautiful Split-Screen (2/3 Code Editor, 1/3 Side Annotation Cards)."""
        ShapeService.draw_header(slide, "مثال حقيقي متكامل بلغة ض", slide_idx)
        
        # Left 2/3 - Main Code Editor
        cb = sd.get("code_block", {})
        code_txt = cb.get("code", "")
        ShapeService.draw_ide_block(slide, Inches(0.5), Inches(1.5), Inches(7.5), Inches(5.2), "حاسبة الجمع البرمجية", code_txt)
        
        # Right 1/3 - Annotations
        boxes = sd.get("boxes", [])
        for i, box in enumerate(boxes[:3]):
            y_pos = Inches(1.5 + i*1.75)
            ShapeService.draw_card(slide, Inches(8.3), y_pos, Inches(4.5), Inches(1.6), ThemeColors.WHITE, ThemeColors.PRIMARY, 1)
            # Vertical green stripe
            stripe = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, Inches(8.3), y_pos, Inches(0.06), Inches(1.6))
            stripe.fill.solid(); stripe.fill.fore_color.rgb = ThemeColors.PRIMARY; stripe.line.fill.background()
            
            # Step badge on the left of each annotation card
            ShapeService.draw_icon_badge(slide, Inches(8.4), y_pos + Inches(0.55), Inches(0.5), inner="dot", fill_color=ThemeColors.PRIMARY, border_color=ThemeColors.PRIMARY)
            ShapeService.draw_diamond(slide, Inches(12.45), y_pos + Inches(0.12), Inches(0.14), ThemeColors.ACCENT)
            
            ShapeService.draw_textbox(slide, Inches(8.5), y_pos + Inches(0.15), Inches(4.1), Inches(0.4), box.get("title", ""), font_size=16, font_color=ThemeColors.cycle(i), bold=True, align=PP_ALIGN.RIGHT)
            ShapeService.draw_textbox(slide, Inches(8.5), y_pos + Inches(0.6), Inches(4.1), Inches(0.9), box.get("desc", ""), font_size=12, font_color=ThemeColors.TEXT_BODY, align=PP_ALIGN.RIGHT)

    @staticmethod
    def render_slide_7_quick_look(slide, sd, slide_idx):
        """Slide 07: Horizontal customized code widgets displaying language syntax."""
        ShapeService.draw_header(slide, "كيف تبدو لغة «ض»؟ (أمثلة سريعة)", slide_idx)
        examples = sd.get("items", [])
        col_w = Inches(2.8)
        gap = Inches(0.2)
        start_x = Inches(0.5)
        
        for i, item in enumerate(examples[:4]):
            x = start_x + i * (col_w + gap)
            y = Inches(1.8)
            ShapeService.draw_card(slide, x, y, col_w, Inches(4.8), ThemeColors.WHITE, ThemeColors.PRIMARY, 1)
            
            # Top Banner tag
            tag = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, x, y, col_w, Inches(0.5))
            tag.fill.solid(); tag.fill.fore_color.rgb = ThemeColors.PRIMARY; tag.line.fill.background()
            ShapeService.draw_textbox(slide, x, y + Inches(0.05), col_w, Inches(0.4), item.get("t", ""), font_size=12, font_color=ThemeColors.WHITE, bold=True, align=PP_ALIGN.CENTER)
            
            # Code Preview Block (IDE style)
            code_bg = slide.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, x + Inches(0.15), y + Inches(0.7), col_w - Inches(0.3), Inches(3.8))
            code_bg.fill.solid(); code_bg.fill.fore_color.rgb = ThemeColors.CODE_BG; code_bg.line.color.rgb = ThemeColors.BORDER; code_bg.line.width = Pt(0.5)
            
            # IDE mini window dots in the code preview corner
            for j, clr in enumerate([ThemeColors.STATUS_RED, ThemeColors.STATUS_ORANGE, ThemeColors.PRIMARY]):
                md = slide.shapes.add_shape(MSO_SHAPE.OVAL, x + Inches(0.3 + j*0.22), y + Inches(0.85), Inches(0.13), Inches(0.13))
                md.fill.solid(); md.fill.fore_color.rgb = clr; md.line.fill.background()
            
            ShapeService.draw_textbox(slide, x + Inches(0.2), y + Inches(0.85), col_w - Inches(0.4), Inches(3.5), item.get("d", ""), font_size=11, font_color=ThemeColors.SECONDARY, font_name=Typography.CODE, align=PP_ALIGN.LEFT)

    @staticmethod
    def render_slide_8_components_matrix(slide, sd, slide_idx):
        """Slide 08: Elegant 4x2 Rounded Card matrix of compiler components."""
        ShapeService.draw_header(slide, "مكونات منظومة اللغة التقنية", slide_idx)
        components = sd.get("items", [])
        
        col_w = Inches(2.8)
        row_h = Inches(2.2)
        gap_x = Inches(0.2)
        gap_y = Inches(0.2)
        start_x = Inches(0.5)
        start_y = Inches(1.6)
        
        for idx, item in enumerate(components[:8]):
            c = idx % 4
            r = idx // 4
            x = start_x + c * (col_w + gap_x)
            y = start_y + r * (row_h + gap_y)
            
            ShapeService.draw_card(slide, x, y, col_w, row_h, ThemeColors.WHITE, ThemeColors.PRIMARY, 1)
            # Colored left indicator tag
            tag = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, x, y, Inches(0.06), row_h)
            tag.fill.solid(); tag.fill.fore_color.rgb = ThemeColors.PRIMARY; tag.line.fill.background()
            
            # Icon badge in the corner of each card
            ShapeService.draw_icon_badge(slide, x + col_w - Inches(0.55), y + Inches(0.15), Inches(0.42), inner="diamond", fill_color=ThemeColors.BG_CARD, border_color=ThemeColors.PRIMARY)
            ShapeService.draw_plus(slide, x + Inches(0.18), y + Inches(0.18), Inches(0.16), ThemeColors.ACCENT)
            
            ShapeService.draw_textbox(slide, x + Inches(0.15), y + Inches(0.15), col_w - Inches(0.3), Inches(0.4), item.get("t", ""), font_size=13, font_color=ThemeColors.PRIMARY, bold=True, align=PP_ALIGN.RIGHT)
            ShapeService.draw_textbox(slide, x + Inches(0.15), y + Inches(0.65), col_w - Inches(0.3), Inches(1.4), item.get("d", ""), font_size=10, font_color=ThemeColors.TEXT_BODY, align=PP_ALIGN.RIGHT)

    @staticmethod
    def render_slide_10_lexer(slide, sd, slide_idx):
        """Slide 10: Customized Lexer Split Layout."""
        ShapeService.draw_header(slide, "Lexer — المحلل اللفظي لـ «ض»", slide_idx)
        
        # Left Side Panel - Bullet Descriptions
        li = sd.get("list", {})
        ShapeService.draw_card(slide, Inches(0.5), Inches(1.5), Inches(6.0), Inches(5.2), ThemeColors.BG_CARD, ThemeColors.PRIMARY)
        ShapeService.draw_textbox(slide, Inches(0.7), Inches(1.7), Inches(5.6), Inches(0.5), li.get("label", "الدور الوظيفي"), font_size=16, font_color=ThemeColors.PRIMARY, bold=True, align=PP_ALIGN.RIGHT)
        ShapeService.draw_bullet_list(slide, Inches(0.7), Inches(2.3), Inches(5.6), Inches(4.1), li.get("items", []), font_size=13)
        
        # Right Side Panel - Custom Code Inputs/Outputs (converts string -> tokens)
        ShapeService.draw_ide_block(slide, Inches(7.0), Inches(1.5), Inches(5.8), Inches(1.8), "كود المصدر المدخل", "صحيح س = 10 ؛")
        
        # Down Pointing Vector Arrow
        UI_arrow = slide.shapes.add_shape(MSO_SHAPE.DOWN_ARROW, Inches(9.65), Inches(3.45), Inches(0.4), Inches(0.4))
        UI_arrow.fill.solid(); UI_arrow.fill.fore_color.rgb = ThemeColors.PRIMARY; UI_arrow.line.fill.background()
        
        tokens_code = "TOKEN_KEYWORD  \"صحيح\"\nTOKEN_IDENTIFIER  \"س\"\nTOKEN_EQUALS  \"=\"\nTOKEN_NUMBER  \"10\"\nTOKEN_SEMICOLON  \"؛\""
        ShapeService.draw_ide_block(slide, Inches(7.0), Inches(4.0), Inches(5.8), Inches(2.7), "الرموز الناتجة (Tokens)", tokens_code)
