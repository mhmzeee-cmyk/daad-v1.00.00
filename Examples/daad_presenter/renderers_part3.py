# -*- coding: utf-8 -*-
"""
================================================================================
DAAD PRESENTATION ENGINE - CUSTOM RENDERING SERVICES (SLIDES 21-29)
================================================================================
"""

from pptx.util import Inches, Pt
from pptx.enum.shapes import MSO_SHAPE
from pptx.enum.text import PP_ALIGN
from daad_presenter.theme import ThemeColors, Dimensions, Typography
from daad_presenter.shapes import ShapeService

class RenderersPart3:
    @staticmethod
    def render_slide_21_ir_why(slide, sd, slide_idx):
        """Slide 21: Customized Grid for Intermediate Representation."""
        ShapeService.draw_header(slide, "لماذا نحتاج طبقة وسيطة (IR)؟", slide_idx)
        items = sd.get("items", [])
        
        col_w = Inches(5.8)
        row_h = Inches(2.2)
        start_x = Inches(0.5)
        start_y = Inches(1.8)
        
        for idx, item in enumerate(items[:4]):
            c = idx % 2
            r = idx // 2
            x = start_x + c * (col_w + Inches(0.4))
            y = start_y + r * (row_h + Inches(0.4))
            
            ShapeService.draw_card(slide, x, y, col_w, row_h, ThemeColors.WHITE, ThemeColors.PRIMARY, 1)
            # Custom top bar
            stripe = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, x, y, col_w, Inches(0.12))
            stripe.fill.solid(); stripe.fill.fore_color.rgb = ThemeColors.PRIMARY; stripe.line.fill.background()
            
            # Corner icon badge for each card
            ShapeService.draw_icon_badge(slide, x + col_w - Inches(0.55), y + Inches(0.15), Inches(0.42), inner="diamond", fill_color=ThemeColors.BG_CARD, border_color=ThemeColors.PRIMARY)
            ShapeService.draw_plus(slide, x + Inches(0.18), y + Inches(0.18), Inches(0.16), ThemeColors.ACCENT)
            
            ShapeService.draw_textbox(slide, x + Inches(0.2), y + Inches(0.2), col_w - Inches(0.4), Inches(0.4), item.get("t", ""), font_size=14, font_color=ThemeColors.PRIMARY, bold=True, align=PP_ALIGN.RIGHT)
            ShapeService.draw_textbox(slide, x + Inches(0.2), y + Inches(0.7), col_w - Inches(0.4), Inches(1.3), item.get("d", ""), font_size=11, font_color=ThemeColors.TEXT_BODY, align=PP_ALIGN.RIGHT)

    @staticmethod
    def render_slide_22_ir_conversion(slide, sd, slide_idx):
        """Slide 22: Code Box Conversion display."""
        ShapeService.draw_header(slide, "كيف يمكن أن يحدث التحويل؟ (تصور الـ IR)", slide_idx)
        
        # Left Side: Custom Note text block
        note = sd.get("note", {}).get("text", "تمثيل وسيط (IR) للتعليمات البرمجية:")
        ShapeService.draw_card(slide, Inches(0.5), Inches(1.5), Inches(5.5), Inches(5.2), ThemeColors.BG_CARD, ThemeColors.PRIMARY)
        ShapeService.draw_textbox(slide, Inches(0.7), Inches(1.8), Inches(5.1), Inches(1.0), "طبقة الـ IR:", font_size=18, font_color=ThemeColors.PRIMARY, bold=True, align=PP_ALIGN.RIGHT)
        ShapeService.draw_textbox(slide, Inches(0.7), Inches(2.6), Inches(5.1), Inches(3.5), note + "\n\nتسمح طبقة الـ IR بفصل الكود المصدري عن بنية المعالجات المادية المختلفة (x86, ARM, RISC-V) لتسهيل عمليات التحسين والـ Code Generation المستقلة.", font_size=13, font_color=ThemeColors.TEXT_BODY, align=PP_ALIGN.RIGHT)
        
        # Right Side: Code block
        cb = sd.get("code_block", {})
        ShapeService.draw_ide_block(slide, Inches(6.5), Inches(1.5), Inches(6.3), Inches(5.2), "تصور كود الـ IR المولد", cb.get("code", ""))

    @staticmethod
    def render_slide_23_processors(slide, sd, slide_idx):
        """Slide 23: Processors future compatibility with vector list."""
        ShapeService.draw_header(slide, "دعم معماريات المعالجات مستقبلاً", slide_idx)
        
        # Left Side: LLVM note box
        note_text = sd.get("note", {}).get("text", "الخيار الأفضل هو استخدام LLVM لدعم جميع هذه المعالجات تلقائياً")
        ShapeService.draw_card(slide, Inches(0.5), Inches(1.5), Inches(5.5), Inches(5.2), ThemeColors.BG_CARD, ThemeColors.PRIMARY)
        ShapeService.draw_textbox(slide, Inches(0.7), Inches(1.8), Inches(5.1), Inches(1.0), "خطة LLVM الاستراتيجية:", font_size=18, font_color=ThemeColors.PRIMARY, bold=True, align=PP_ALIGN.RIGHT)
        ShapeService.draw_textbox(slide, Inches(0.7), Inches(2.8), Inches(5.1), Inches(3.0), note_text + "\n\nيتيح محرك LLVM للمشروع توليد لغات آلة متناهية الدقة والسرعة لمعالجات x86 و ARM مباشرة دون الحاجة لإعادة كتابة الأجزاء المتقدمة من المترجم من الصفر.", font_size=13, font_color=ThemeColors.TEXT_BODY, align=PP_ALIGN.RIGHT)
        
        # Right Side: 3 custom vertical boxes representing processors
        processors = sd.get("items", [])
        for i, item in enumerate(processors[:3]):
            y_pos = Inches(1.5 + i*1.75)
            ShapeService.draw_card(slide, Inches(6.5), y_pos, Inches(6.3), Inches(1.6), ThemeColors.WHITE, ThemeColors.PRIMARY, 1)
            # Accent square
            sq = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, Inches(6.5), y_pos, Inches(0.06), Inches(1.6))
            sq.fill.solid(); sq.fill.fore_color.rgb = ThemeColors.PRIMARY; sq.line.fill.background()
            
            ShapeService.draw_textbox(slide, Inches(6.7), y_pos + Inches(0.15), Inches(5.9), Inches(0.4), item.get("t", ""), font_size=14, font_color=ThemeColors.PRIMARY, bold=True, align=PP_ALIGN.RIGHT)
            ShapeService.draw_textbox(slide, Inches(6.7), y_pos + Inches(0.6), Inches(5.9), Inches(0.9), item.get("d", ""), font_size=11, font_color=ThemeColors.TEXT_BODY, align=PP_ALIGN.RIGHT)

    @staticmethod
    def render_slide_24_roadmap(slide, sd, slide_idx):
        """Slide 24: Customized vertical stage items for roadmap."""
        ShapeService.draw_header(slide, "خارطة طريق تطوير لغة ض المتكاملة", slide_idx)
        stages = sd.get("items", [])
        
        col_w = Inches(3.8)
        row_h = Inches(2.3)
        gap_x = Inches(0.4)
        gap_y = Inches(0.4)
        start_x = Inches(0.5)
        start_y = Inches(1.8)
        
        for idx, item in enumerate(stages[:6]):
            c = idx % 3
            r = idx // 3
            x = start_x + c * (col_w + gap_x)
            y = start_y + r * (row_h + gap_y)
            
            ShapeService.draw_card(slide, x, y, col_w, row_h, ThemeColors.WHITE, ThemeColors.PRIMARY, 1)
            # Custom top-stripe
            stripe = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, x, y, col_w, Inches(0.12))
            stripe.fill.solid(); stripe.fill.fore_color.rgb = ThemeColors.PRIMARY; stripe.line.fill.background()
            
            # Corner icon badge for each card
            ShapeService.draw_icon_badge(slide, x + col_w - Inches(0.55), y + Inches(0.15), Inches(0.42), inner="square", fill_color=ThemeColors.BG_CARD, border_color=ThemeColors.PRIMARY)
            ShapeService.draw_diamond(slide, x + Inches(0.18), y + Inches(0.2), Inches(0.18), ThemeColors.ACCENT)
            
            ShapeService.draw_textbox(slide, x + Inches(0.2), y + Inches(0.25), col_w - Inches(0.4), Inches(0.4), item.get("t", ""), font_size=13, font_color=ThemeColors.PRIMARY, bold=True, align=PP_ALIGN.RIGHT)
            ShapeService.draw_textbox(slide, x + Inches(0.2), y + Inches(0.7), col_w - Inches(0.4), Inches(1.4), item.get("d", ""), font_size=10, font_color=ThemeColors.TEXT_BODY, align=PP_ALIGN.RIGHT)

    @staticmethod
    def render_slide_25_strengths(slide, sd, slide_idx):
        """Slide 25: Grid for strengths."""
        ShapeService.draw_header(slide, "نقاط القوة والمميزات الاستراتيجية", slide_idx)
        strengths = sd.get("items", [])
        
        col_w = Inches(3.8)
        row_h = Inches(2.3)
        gap_x = Inches(0.4)
        gap_y = Inches(0.4)
        start_x = Inches(0.5)
        start_y = Inches(1.8)
        
        for idx, item in enumerate(strengths[:6]):
            c = idx % 3
            r = idx // 3
            x = start_x + c * (col_w + gap_x)
            y = start_y + r * (row_h + gap_y)
            
            ShapeService.draw_card(slide, x, y, col_w, row_h, ThemeColors.WHITE, ThemeColors.PRIMARY, 1)
            # Small icon circle on right
            icon = slide.shapes.add_shape(MSO_SHAPE.OVAL, x + col_w - Inches(0.85), y + Inches(0.2), Inches(0.65), Inches(0.65))
            icon.fill.solid(); icon.fill.fore_color.rgb = ThemeColors.BG_CARD; icon.line.color.rgb = ThemeColors.PRIMARY
            # Inner geometric icon inside the circle
            ShapeService.draw_diamond(slide, x + col_w - Inches(0.68), y + Inches(0.37), Inches(0.16), ThemeColors.ACCENT)
            # Corner badge accent
            ShapeService.draw_plus(slide, x + Inches(0.18), y + Inches(0.2), Inches(0.16), ThemeColors.ACCENT)
            
            ShapeService.draw_textbox(slide, x + Inches(0.2), y + Inches(0.25), col_w - Inches(1.2), Inches(0.4), item.get("t", ""), font_size=13, font_color=ThemeColors.PRIMARY, bold=True, align=PP_ALIGN.RIGHT)
            ShapeService.draw_textbox(slide, x + Inches(0.2), y + Inches(0.9), col_w - Inches(0.4), Inches(1.3), item.get("d", ""), font_size=10, font_color=ThemeColors.TEXT_BODY, align=PP_ALIGN.RIGHT)

    @staticmethod
    def render_slide_26_challenges(slide, sd, slide_idx):
        """Slide 26: Grid for technical challenges."""
        ShapeService.draw_header(slide, "التحديات البرمجية والعقبات التقنية", slide_idx)
        challenges = sd.get("items", [])
        
        col_w = Inches(3.8)
        row_h = Inches(2.3)
        gap_x = Inches(0.4)
        gap_y = Inches(0.4)
        start_x = Inches(0.5)
        start_y = Inches(1.8)
        
        for idx, item in enumerate(challenges[:6]):
            c = idx % 3
            r = idx // 3
            x = start_x + c * (col_w + gap_x)
            y = start_y + r * (row_h + gap_y)
            
            ShapeService.draw_card(slide, x, y, col_w, row_h, ThemeColors.WHITE, ThemeColors.BORDER, 0.5)
            # Left stripe in orange indicating alert/warning
            stripe = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, x, y, Inches(0.06), row_h)
            stripe.fill.solid(); stripe.fill.fore_color.rgb = ThemeColors.STATUS_ORANGE; stripe.line.fill.background()
            
            # Corner badge with alert dot
            ShapeService.draw_icon_badge(slide, x + col_w - Inches(0.55), y + Inches(0.15), Inches(0.42), inner="dot", fill_color=ThemeColors.WHITE, border_color=ThemeColors.STATUS_ORANGE)
            ShapeService.draw_diamond(slide, x + Inches(0.18), y + Inches(0.2), Inches(0.18), ThemeColors.STATUS_ORANGE)
            
            ShapeService.draw_textbox(slide, x + Inches(0.2), y + Inches(0.2), col_w - Inches(0.3), Inches(0.4), item.get("t", ""), font_size=13, font_color=ThemeColors.STATUS_ORANGE, bold=True, align=PP_ALIGN.RIGHT)
            ShapeService.draw_textbox(slide, x + Inches(0.2), y + Inches(0.7), col_w - Inches(0.3), Inches(1.4), item.get("d", ""), font_size=10, font_color=ThemeColors.TEXT_BODY, align=PP_ALIGN.RIGHT)

    @staticmethod
    def render_slide_27_future_vision(slide, sd, slide_idx):
        """Slide 27: Flowchart displaying the ultimate future target of DAAD compiler."""
        ShapeService.draw_header(slide, "أين يمكن أن تصل «ض»؟ (الرؤية المستقبلية)", slide_idx)
        
        # Top banner note
        note_text = sd.get("note", {}).get("text", "")
        ShapeService.draw_card(slide, Inches(0.5), Inches(1.3), Inches(12.333), Inches(0.8), ThemeColors.BG_CARD, ThemeColors.PRIMARY)
        ShapeService.draw_textbox(slide, Inches(0.7), Inches(1.4), Inches(11.933), Inches(0.6), note_text, font_size=13, font_color=ThemeColors.PRIMARY, bold=True, align=PP_ALIGN.CENTER)
        
        # Horizontal flow
        flow = sd.get("flow", {})
        steps = flow.get("steps", [])
        step_w = Inches(1.3)
        gap = Inches(0.2)
        total_steps = len(steps)
        start_x = (Dimensions.SCREEN_W - (step_w * total_steps + gap * (total_steps - 1))) / 2
        y_pos = Inches(3.2)
        
        for i, step in enumerate(steps):
            x = start_x + i * (step_w + gap)
            bg = ThemeColors.PRIMARY if step.get("highlight") else ThemeColors.WHITE
            tx = ThemeColors.WHITE if step.get("highlight") else ThemeColors.TEXT_DARK
            border = ThemeColors.PRIMARY if step.get("highlight") else ThemeColors.BORDER
            
            ShapeService.draw_card(slide, x, y_pos, step_w, Inches(1.4), bg, border, 1)
            ShapeService.draw_textbox(slide, x, y_pos + Inches(0.4), step_w, Inches(0.8), step.get("text", ""), font_size=11, font_color=tx, bold=True, align=PP_ALIGN.CENTER)
            
            # Step circle badge
            num = slide.shapes.add_shape(MSO_SHAPE.OVAL, x + Inches(0.45), y_pos + Inches(-0.35), Inches(0.4), Inches(0.4))
            num.fill.solid(); num.fill.fore_color.rgb = ThemeColors.SECONDARY if step.get("highlight") else ThemeColors.BG_CARD
            num.line.fill.background()
            ShapeService.draw_textbox(slide, x + Inches(0.45), y_pos + Inches(-0.32), Inches(0.4), Inches(0.35), f"{i+1}", font_size=10, font_color=ThemeColors.WHITE if step.get("highlight") else ThemeColors.PRIMARY, bold=True, align=PP_ALIGN.CENTER)
            
            # Connectors
            if i < total_steps - 1:
                ShapeService.draw_arrow(slide, x + step_w + Inches(0.02), y_pos + Inches(0.55), Inches(0.16), Inches(0.3))

    @staticmethod
    def render_slide_28_conclusion(slide, sd, slide_idx):
        """Slide 28: Matches Target Image 03 footer (Conclusion list + huge citation quotes marks)."""
        ShapeService.draw_header(slide, sd.get("title", "الخاتمة والخلاصة"), slide_idx)
        items = sd.get("items", [])
        
        # 4 big horizontal columns representing conclusion highlights
        for i, item in enumerate(items[:4]):
            y_pos = Inches(1.5 + i*1.0)
            ShapeService.draw_card(slide, Inches(0.5), y_pos, Inches(12.333), Inches(0.85), ThemeColors.WHITE, ThemeColors.BORDER, 0.5)
            # Left stripe tag
            stripe = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, Inches(0.5), y_pos, Inches(0.06), Inches(0.85))
            stripe.fill.solid(); stripe.fill.fore_color.rgb = ThemeColors.PRIMARY; stripe.line.fill.background()
            
            ShapeService.draw_textbox(slide, Inches(0.8), y_pos + Inches(0.18), Inches(11.733), Inches(0.5), item, font_size=13, font_color=ThemeColors.TEXT_BODY, bold=True, align=PP_ALIGN.RIGHT)

        # Huge citation quotes marks banner (Slide 03 style)
        banner_y = Inches(5.7)
        ShapeService.draw_card(slide, Inches(0.5), banner_y, Inches(12.333), Inches(1.2), ThemeColors.BG_CARD, ThemeColors.PRIMARY)
        # Giant green quotes mark
        quotes = slide.shapes.add_textbox(Inches(0.7), banner_y + Inches(0.15), Inches(1.5), Inches(0.9))
        quotes.text_frame.paragraphs[0].text = "“"
        quotes.text_frame.paragraphs[0].font.size = Pt(80)
        quotes.text_frame.paragraphs[0].font.color.rgb = ThemeColors.PRIMARY
        quotes.text_frame.paragraphs[0].font.bold = True
        
        quote_text = sd.get("quote", "«ض — من البرمجة بالعربية إلى بناء لغة متكاملة»")
        ShapeService.draw_textbox(slide, Inches(1.8), banner_y + Inches(0.35), Inches(10.5), Inches(0.6), quote_text, font_size=16, font_color=ThemeColors.PRIMARY, bold=True, align=PP_ALIGN.RIGHT)

    @staticmethod
    def render_slide_29_sources(slide, sd, slide_idx):
        """Slide 29: Final slide displaying all bibliography sources."""
        ShapeService.draw_header(slide, sd.get("heading", "مصادر ومراجع المعلومات الفنية"), slide_idx)
        sources = sd.get("items", [])
        
        # Dual list panels for sources list
        col_w = Inches(5.9)
        start_x = Inches(0.5)
        
        for col_idx in range(2):
            x = start_x + col_idx*6.433
            ShapeService.draw_card(slide, x, Inches(1.5), col_w, Inches(5.2), ThemeColors.WHITE, ThemeColors.PRIMARY, 1)
            # Top stripe
            stripe = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, x, Inches(1.5), col_w, Inches(0.12))
            stripe.fill.solid(); stripe.fill.fore_color.rgb = ThemeColors.PRIMARY; stripe.line.fill.background()
            
            subset = sources[col_idx*5:(col_idx+1)*5]
            ShapeService.draw_bullet_list(slide, x + Inches(0.2), Inches(1.8), col_w - Inches(0.4), Inches(4.7), subset, font_size=12)

    @staticmethod
    def render_generic_grid(slide, sd, slide_idx):
        """Standard beautiful Grid Layout for slides with items (e.g. Slide 04, 05, etc)."""
        heading_data = sd.get("heading", {})
        if isinstance(heading_data, dict):
            slide_title = heading_data.get("text", sd.get("title", ""))
        else:
            slide_title = heading_data if heading_data else sd.get("title", "")
            
        ShapeService.draw_header(slide, slide_title, slide_idx)
        
        items = sd.get("items", [])
        if not items:
            # Fallback for slides with text/lists or custom blocks (e.g., Code Block or Columns)
            if "code_block" in sd:
                cb = sd["code_block"]
                ShapeService.draw_ide_block(slide, Inches(1.5), Inches(1.6), Inches(10.3), Inches(5.2), "محرر ض", cb.get("code", ""))
                return
            elif "list" in sd:
                li = sd["list"]
                ShapeService.draw_card(slide, Inches(1.5), Inches(1.6), Inches(10.3), Inches(5.2), ThemeColors.BG_CARD, ThemeColors.PRIMARY)
                ShapeService.draw_textbox(slide, Inches(1.7), Inches(1.8), Inches(9.9), Inches(0.5), li.get("label", ""), font_size=16, font_color=ThemeColors.PRIMARY, bold=True)
                ShapeService.draw_bullet_list(slide, Inches(1.7), Inches(2.4), Inches(9.9), Inches(4.2), li.get("items", []), font_size=13)
                return
            else:
                # Flat Text fallback
                ShapeService.draw_card(slide, Inches(1.5), Inches(2.0), Inches(10.3), Inches(4.0), ThemeColors.BG_CARD, ThemeColors.PRIMARY)
                ShapeService.draw_textbox(slide, Inches(1.8), Inches(2.3), Inches(9.7), Inches(3.4), sd.get("title", sd.get("heading", {}).get("text", "")), font_size=20, font_color=ThemeColors.PRIMARY, bold=True, align=PP_ALIGN.CENTER)
                return

        columns = 2
        if len(items) > 4:
            columns = 3
            
        rows_count = (len(items) + columns - 1) // columns
        if rows_count <= 0:
            rows_count = 1
            
        card_w = Inches(11.333 / columns - 0.2)
        card_h = Inches(4.5 / rows_count - 0.2)
        start_x = Inches(1.0)
        start_y = Inches(1.8)
        
        for idx, item in enumerate(items):
            c = idx % columns
            r = idx // columns
            x = start_x + c * (card_w + Inches(0.3))
            y = start_y + r * (card_h + Inches(0.3))
            
            ShapeService.draw_card(slide, x, y, card_w, card_h, ThemeColors.WHITE, ThemeColors.PRIMARY, 1)
            
            if isinstance(item, dict):
                # Title
                ShapeService.draw_textbox(slide, x + Inches(0.2), y + Inches(0.15), card_w - Inches(0.4), Inches(0.4), item.get("t", ""), font_size=14, font_color=ThemeColors.PRIMARY, bold=True, align=PP_ALIGN.RIGHT)
                # Line separator
                line = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, x + Inches(0.2), y + Inches(0.6), card_w - Inches(0.4), Inches(0.01))
                line.fill.solid(); line.fill.fore_color.rgb = ThemeColors.BORDER; line.line.fill.background()
                # Desc
                ShapeService.draw_textbox(slide, x + Inches(0.2), y + Inches(0.68), card_w - Inches(0.4), card_h - Inches(0.85), item.get("d", ""), font_size=11, font_color=ThemeColors.TEXT_BODY, align=PP_ALIGN.RIGHT)
            else:
                # Flat string item (simple text bullet)
                ShapeService.draw_textbox(slide, x + Inches(0.2), y + Inches(0.2), card_w - Inches(0.4), card_h - Inches(0.4), item, font_size=12, font_color=ThemeColors.TEXT_BODY, bold=True, align=PP_ALIGN.RIGHT)
