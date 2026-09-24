# -*- coding: utf-8 -*-
"""
================================================================================
DAAD PRESENTATION ENGINE - CUSTOM RENDERING SERVICES (SLIDES 11-20)
================================================================================
Highly comprehensive, custom slide-by-slide layout system.
================================================================================
"""

from pptx.util import Inches
from pptx.enum.shapes import MSO_SHAPE
from pptx.enum.text import PP_ALIGN
from daad_presenter.theme import ThemeColors, Typography, Dimensions
from daad_presenter.shapes import ShapeService

class RenderersPart2:
    @staticmethod
    def render_slide_9_flowchart(slide, sd, slide_idx):
        """Slide 09: Dynamic horizontal flowchart with large clear steps and zero overlap."""
        ShapeService.draw_header(slide, "من النص إلى التنفيذ (مسار العمليات)", slide_idx)
        
        flow = sd.get("flow", {})
        steps = flow.get("steps", [])
        
        step_w = Inches(1.3)
        gap = Inches(0.2)
        total_steps = len(steps)
        start_x = (Dimensions.SCREEN_W - (step_w * total_steps + gap * (total_steps - 1))) / 2
        y_pos = Inches(2.8)
        
        # Concept map flowchart builder
        for i, step in enumerate(steps):
            x = start_x + i * (step_w + gap)
            bg_color = ThemeColors.PRIMARY if step.get("highlight") else ThemeColors.BG_CARD
            border_color = ThemeColors.SECONDARY if step.get("highlight") else ThemeColors.BORDER
            tx_color = ThemeColors.WHITE if step.get("highlight") else ThemeColors.TEXT_DARK
            
            # Box Shape
            ShapeService.draw_card(slide, x, y_pos, step_w, Inches(1.2), bg_color, border_color)
            ShapeService.draw_textbox(slide, x, y_pos + Inches(0.3), step_w, Inches(0.6), step["text"], font_size=11, font_color=tx_color, bold=True, align=PP_ALIGN.CENTER)
            
            # Step numbering
            ShapeService.draw_textbox(slide, x, y_pos - Inches(0.4), step_w, Inches(0.3), f"الخطوة {i+1}", font_size=10, font_color=ThemeColors.PRIMARY, bold=True, align=PP_ALIGN.CENTER)
            
            # Arrow Connector
            if i < total_steps - 1:
                arrow_x = x + step_w + Inches(0.02)
                ShapeService.draw_arrow(slide, arrow_x, y_pos + Inches(0.45), Inches(0.16), Inches(0.3))
                
        # Description or note below
        note = flow.get("note", {}).get("text", "مسار الويب: ض -> JavaScript -> تنفيذ في المتصفح")
        ShapeService.draw_textbox(slide, Inches(1), Inches(4.8), Inches(11.333), Inches(0.6), note, font_size=14, font_color=ThemeColors.SECONDARY, bold=True, align=PP_ALIGN.CENTER)

    @staticmethod
    def render_slide_11_parser(slide, sd, slide_idx):
        """Slide 11: Customized Parser Layout (Description + Flow Columns)."""
        ShapeService.draw_header(slide, "Parser — محلل القواعد والتركيب", slide_idx)
        
        # Left Panel Description
        li = sd.get("list", {})
        ShapeService.draw_card(slide, Inches(0.5), Inches(1.5), Inches(6.0), Inches(5.2), ThemeColors.BG_CARD, ThemeColors.PRIMARY)
        ShapeService.draw_textbox(slide, Inches(0.7), Inches(1.7), Inches(5.6), Inches(0.5), li.get("label", "الآلية"), font_size=16, font_color=ThemeColors.PRIMARY, bold=True, align=PP_ALIGN.RIGHT)
        ShapeService.draw_bullet_list(slide, Inches(0.7), Inches(2.3), Inches(5.6), Inches(4.1), li.get("items", []), font_size=13)
        
        # Right Panel Custom Flow Structure
        flow = sd.get("flow", {})
        steps = flow.get("steps", [])
        
        # 3 Stacked flowing boxes
        for i, step in enumerate(steps[:3]):
            y_pos = Inches(1.5 + i*1.75)
            bg = ThemeColors.PRIMARY if step.get("highlight") else ThemeColors.WHITE
            tx = ThemeColors.WHITE if step.get("highlight") else ThemeColors.TEXT_DARK
            ShapeService.draw_card(slide, Inches(7.5), y_pos, Inches(4.5), Inches(1.2), bg, ThemeColors.PRIMARY, 1)
            ShapeService.draw_textbox(slide, Inches(7.5), y_pos + Inches(0.35), Inches(4.5), Inches(0.5), step.get("text", ""), font_size=14, font_color=tx, bold=True, align=PP_ALIGN.CENTER)
            
            # Connector arrow downward
            if i < len(steps[:3]) - 1:
                arr = slide.shapes.add_shape(MSO_SHAPE.DOWN_ARROW, Inches(9.55), y_pos + Inches(1.25), Inches(0.4), Inches(0.4))
                arr.fill.solid(); arr.fill.fore_color.rgb = ThemeColors.PRIMARY; arr.line.fill.background()

    @staticmethod
    def render_slide_12_ast(slide, sd, slide_idx):
        """Slide 12: Complex AST Tree Split (Expressions vs Statements columns)."""
        ShapeService.draw_header(slide, "AST — شجرة النحو المجردة للغة ض", slide_idx)
        
        # Top banner note
        note = sd.get("note", {}).get("text", "29 نوع عقدة  ·  Visitor Pattern  ·  دعم كامل لللغة")
        ShapeService.draw_card(slide, Inches(0.5), Inches(1.3), Inches(12.333), Inches(0.6), ThemeColors.BG_CARD, ThemeColors.PRIMARY)
        ShapeService.draw_textbox(slide, Inches(0.7), Inches(1.35), Inches(11.933), Inches(0.5), note, font_size=13, font_color=ThemeColors.PRIMARY, bold=True, align=PP_ALIGN.CENTER)
        
        # Dual Columns
        cols = sd.get("columns", [])
        for col_idx, col in enumerate(cols[:2]):
            x_pos = Inches(0.5 + col_idx*6.25)
            ShapeService.draw_card(slide, x_pos, Inches(2.1), Inches(6.0), Inches(4.6), ThemeColors.WHITE, ThemeColors.PRIMARY, 1)
            # Custom top-stripe on columns
            stripe = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, x_pos, Inches(2.1), Inches(6.0), Inches(0.12))
            stripe.fill.solid(); stripe.fill.fore_color.rgb = ThemeColors.PRIMARY; stripe.line.fill.background()
            
            ShapeService.draw_textbox(slide, x_pos + Inches(0.2), Inches(2.35), Inches(5.6), Inches(0.5), col.get("title", ""), font_size=15, font_color=ThemeColors.PRIMARY, bold=True, align=PP_ALIGN.RIGHT)
            ShapeService.draw_bullet_list(slide, x_pos + Inches(0.2), Inches(2.95), Inches(5.6), Inches(3.6), col.get("items", []), font_size=12)

    @staticmethod
    def render_slide_13_type_system(slide, sd, slide_idx):
        """Slide 13: Type System custom rendering with exact row-by-row mapping table."""
        ShapeService.draw_header(slide, "نظام الأنواع في لغة ض", slide_idx)
        
        # Header Box Title
        header_y = Inches(1.3)
        ShapeService.draw_card(slide, Inches(0.5), header_y, Inches(12.333), Inches(0.5), ThemeColors.PRIMARY, None)
        headers = ["الاستخدام", "في C++", "في ض"]
        col_widths = [Inches(5.0), Inches(3.5), Inches(3.833)]
        start_x = Inches(0.5)
        
        # Draw Headers
        for h_idx, h in enumerate(headers):
            x = start_x + sum(col_widths[:h_idx])
            ShapeService.draw_textbox(slide, x, header_y + Inches(0.05), col_widths[h_idx], Inches(0.4), h, font_size=13, font_color=ThemeColors.WHITE, bold=True, align=PP_ALIGN.CENTER)
            
        # Draw custom rows
        rows = sd.get("table", {}).get("rows", [])
        for r_idx, row in enumerate(rows[:8]):
            y_pos = Inches(1.9 + r_idx*0.56)
            bg = ThemeColors.BG_CARD if r_idx % 2 == 0 else ThemeColors.WHITE
            ShapeService.draw_card(slide, Inches(0.5), y_pos, Inches(12.333), Inches(0.52), bg, ThemeColors.BORDER, 0.5)
            
            for c_idx, cell in enumerate(row[:3]):
                x = start_x + sum(col_widths[:c_idx])
                text_clr = ThemeColors.PRIMARY if c_idx == 2 else ThemeColors.TEXT_BODY
                bold = True if c_idx == 2 else False
                ShapeService.draw_textbox(slide, x, y_pos + Inches(0.06), col_widths[c_idx], Inches(0.4), cell.get("text", ""), font_size=11, font_color=text_clr, bold=bold, align=PP_ALIGN.CENTER)

        # Bottom note banner
        note_text = sd.get("note", {}).get("text", "لا يوجد Type Checker مستقل — التحقق يتم عبر مترجم C++")
        ShapeService.draw_textbox(slide, Inches(0.5), Inches(6.5), Inches(12.333), Inches(0.4), note_text, font_size=12, font_color=ThemeColors.TEXT_MUTED, align=PP_ALIGN.CENTER)

    @staticmethod
    def render_slide_14_stdlib(slide, sd, slide_idx):
        """Slide 14: Standard Library & Runtime customized dual panel."""
        ShapeService.draw_header(slide, "المكتبة القياسية و Runtime للغة ض", slide_idx)
        
        cols = sd.get("columns", [])
        for col_idx, col in enumerate(cols[:2]):
            x_pos = Inches(0.5 + col_idx*6.25)
            ShapeService.draw_card(slide, x_pos, Inches(1.5), Inches(6.0), Inches(5.2), ThemeColors.WHITE, ThemeColors.PRIMARY, 1)
            # Custom top-stripe on columns
            stripe = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, x_pos, Inches(1.5), Inches(6.0), Inches(0.12))
            stripe.fill.solid(); stripe.fill.fore_color.rgb = ThemeColors.PRIMARY; stripe.line.fill.background()
            
            ShapeService.draw_textbox(slide, x_pos + Inches(0.2), Inches(1.85), Inches(5.6), Inches(0.5), col.get("title", ""), font_size=15, font_color=ThemeColors.PRIMARY, bold=True, align=PP_ALIGN.RIGHT)
            ShapeService.draw_bullet_list(slide, x_pos + Inches(0.2), Inches(2.45), Inches(5.6), Inches(4.1), col.get("items", []), font_size=13)

    @staticmethod
    def render_slide_15_status(slide, sd, slide_idx):
        """Slide 15: Gorgeous 4-column Status Grid with status colors."""
        ShapeService.draw_header(slide, "حالة التطوير الحالية للمشروع", slide_idx)
        
        status_cols = sd.get("status_grid", [])
        col_w = Inches(2.8)
        gap = Inches(0.2)
        start_x = Inches(0.5)
        y_pos = Inches(1.5)
        
        color_map = {
            "green": ThemeColors.PRIMARY,
            "orange": ThemeColors.STATUS_ORANGE,
            "blue": ThemeColors.STATUS_BLUE,
            "red": ThemeColors.STATUS_RED
        }
        
        for idx, (col_title, items, clr_name) in enumerate(status_cols):
            x = start_x + idx * (col_w + gap)
            col_color = color_map.get(clr_name, ThemeColors.PRIMARY)
            
            # Header card
            ShapeService.draw_card(slide, x, y_pos, col_w, Inches(0.5), col_color, None)
            ShapeService.draw_textbox(slide, x, y_pos + Inches(0.05), col_w, Inches(0.4), col_title, font_size=13, font_color=ThemeColors.WHITE, bold=True, align=PP_ALIGN.CENTER)
            
            # Outer Container
            ShapeService.draw_card(slide, x, y_pos + Inches(0.6), col_w, Inches(4.8), ThemeColors.WHITE, col_color, 1)
            
            # Items List
            for item_idx, item in enumerate(items[:7]):
                iy = y_pos + Inches(0.7) + item_idx * Inches(0.6)
                # Small green circle bullet
                bullet = slide.shapes.add_shape(MSO_SHAPE.OVAL, x + col_w - Inches(0.25), iy + Inches(0.1), Inches(0.12), Inches(0.12))
                bullet.fill.solid(); bullet.fill.fore_color.rgb = col_color; bullet.line.fill.background()
                
                ShapeService.draw_textbox(slide, x + Inches(0.1), iy, col_w - Inches(0.4), Inches(0.5), item, font_size=10, font_color=ThemeColors.TEXT_BODY, align=PP_ALIGN.RIGHT)

    @staticmethod
    def render_slide_16_achievements(slide, sd, slide_idx):
        """Slide 16: Gorgeous Achievements (4 Cards in a beautiful staggered layout)."""
        ShapeService.draw_header(slide, "ما تم إنجازه في المشروع", slide_idx)
        achievements = sd.get("items", [])
        
        col_w = Inches(5.8)
        row_h = Inches(2.2)
        start_x = Inches(0.5)
        start_y = Inches(1.8)
        
        for idx, item in enumerate(achievements[:4]):
            c = idx % 2
            r = idx // 2
            x = start_x + c * (col_w + Inches(0.4))
            y = start_y + r * (row_h + Inches(0.4))
            
            ShapeService.draw_card(slide, x, y, col_w, row_h, ThemeColors.WHITE, ThemeColors.PRIMARY, 1)
            # Numeric badge circle
            badge = slide.shapes.add_shape(MSO_SHAPE.OVAL, x + col_w - Inches(1.2), y + Inches(0.2), Inches(0.8), Inches(0.8))
            badge.fill.solid(); badge.fill.fore_color.rgb = ThemeColors.BG_CARD; badge.line.color.rgb = ThemeColors.PRIMARY
            ShapeService.draw_textbox(slide, x + col_w - Inches(1.2), y + Inches(0.4), Inches(0.8), Inches(0.4), f"0{idx+1}", font_size=16, font_color=ThemeColors.PRIMARY, bold=True, align=PP_ALIGN.CENTER)
            
            ShapeService.draw_textbox(slide, x + Inches(0.2), y + Inches(0.2), col_w - Inches(1.5), Inches(0.4), item.get("t", ""), font_size=14, font_color=ThemeColors.PRIMARY, bold=True, align=PP_ALIGN.RIGHT)
            ShapeService.draw_textbox(slide, x + Inches(0.2), y + Inches(0.7), col_w - Inches(1.5), Inches(1.3), item.get("d", ""), font_size=10, font_color=ThemeColors.TEXT_BODY, align=PP_ALIGN.RIGHT)

    @staticmethod
    def render_slide_17_incomplete(slide, sd, slide_idx):
        """Slide 17: Customized list representing technical milestones not completed."""
        ShapeService.draw_header(slide, "ما لم يكتمل بعد في المنظومة", slide_idx)
        incomplete = sd.get("items", [])
        
        for i, item in enumerate(incomplete[:6]):
            y_pos = Inches(1.6 + i*0.85)
            ShapeService.draw_card(slide, Inches(0.5), y_pos, Inches(12.333), Inches(0.75), ThemeColors.WHITE, ThemeColors.BORDER, 0.5)
            
            # Milestone colored bar
            clr = ThemeColors.STATUS_RED if i % 2 == 0 else ThemeColors.STATUS_ORANGE
            bar = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, Inches(0.5), y_pos, Inches(0.06), Inches(0.75))
            bar.fill.solid(); bar.fill.fore_color.rgb = clr; bar.line.fill.background()
            
            ShapeService.draw_textbox(slide, Inches(0.8), y_pos + Inches(0.1), Inches(3.5), Inches(0.5), item.get("t", ""), font_size=13, font_color=clr, bold=True, align=PP_ALIGN.RIGHT)
            ShapeService.draw_textbox(slide, Inches(4.5), y_pos + Inches(0.1), Inches(8.0), Inches(0.5), item.get("d", ""), font_size=11, font_color=ThemeColors.TEXT_BODY, align=PP_ALIGN.RIGHT)

    @staticmethod
    def render_slide_18_now(slide, sd, slide_idx):
        """Slide 18: Horizontal flow displaying the current status of DAAD compile pipeline."""
        ShapeService.draw_header(slide, "لغة ض الآن: محاذاة الترجمة والتحويل", slide_idx)
        
        # Top banner quote
        note_text = sd.get("note", {}).get("text", "لا يوجد في هذا المشروع: أي كود Assembly أو Machine Code")
        ShapeService.draw_card(slide, Inches(0.5), Inches(1.3), Inches(12.333), Inches(0.8), ThemeColors.BG_CARD, ThemeColors.PRIMARY)
        ShapeService.draw_textbox(slide, Inches(0.7), Inches(1.4), Inches(11.933), Inches(0.6), note_text, font_size=13, font_color=ThemeColors.PRIMARY, bold=True, align=PP_ALIGN.CENTER)
        
        # Horizontal flowchart
        flow = sd.get("flow", {})
        steps = flow.get("steps", [])
        step_w = Inches(2.5)
        gap = Inches(0.3)
        start_x = (Dimensions.SCREEN_W - (step_w * len(steps[:4]) + gap * (len(steps[:4])-1))) / 2
        
        for i, step in enumerate(steps[:4]):
            x_pos = start_x + i * (step_w + gap)
            y_pos = Inches(3.0)
            bg = ThemeColors.PRIMARY if step.get("highlight") else ThemeColors.WHITE
            tx = ThemeColors.WHITE if step.get("highlight") else ThemeColors.TEXT_DARK
            
            ShapeService.draw_card(slide, x_pos, y_pos, step_w, Inches(2.2), bg, ThemeColors.PRIMARY, 1)
            ShapeService.draw_textbox(slide, x_pos, y_pos + Inches(0.8), step_w, Inches(0.8), step.get("text", ""), font_size=14, font_color=tx, bold=True, align=PP_ALIGN.CENTER)
            
            # Step badge
            num_badge = slide.shapes.add_shape(MSO_SHAPE.OVAL, x_pos + Inches(1.05), y_pos + Inches(0.15), Inches(0.4), Inches(0.4))
            num_badge.fill.solid(); num_badge.fill.fore_color.rgb = ThemeColors.BG_CARD if not step.get("highlight") else ThemeColors.SECONDARY
            num_badge.line.fill.background()
            ShapeService.draw_textbox(slide, x_pos + Inches(1.05), y_pos + Inches(0.2), Inches(0.4), Inches(0.3), f"0{i+1}", font_size=10, font_color=ThemeColors.PRIMARY if not step.get("highlight") else ThemeColors.WHITE, bold=True, align=PP_ALIGN.CENTER)
            
            # Flow arrow
            if i < len(steps[:4]) - 1:
                ShapeService.draw_arrow(slide, x_pos + step_w + Inches(0.05), y_pos + Inches(0.95), Inches(0.2), Inches(0.3))

    @staticmethod
    def render_slide_19_gap_analysis(slide, sd, slide_idx):
        """Slide 19: High vs Low Level Gap Analysis comparative view."""
        ShapeService.draw_header(slide, "ما الذي لم يتم بعد؟ (تحليل الفجوة التقنية)", slide_idx)
        
        # Dual Large Boxes
        # High Level (Available)
        ShapeService.draw_card(slide, Inches(0.5), Inches(1.5), Inches(5.5), Inches(1.8), ThemeColors.PRIMARY, None)
        ShapeService.draw_textbox(slide, Inches(0.7), Inches(1.7), Inches(5.1), Inches(0.4), "المستوى العالي (مكتمل)", font_size=16, font_color=ThemeColors.WHITE, bold=True, align=PP_ALIGN.RIGHT)
        ShapeService.draw_textbox(slide, Inches(0.7), Inches(2.15), Inches(5.1), Inches(1.0), "مرحلة معالجة الكود المصدري ورموزه اللغوية (Lexer, Parser, AST, CodeGen).", font_size=11, font_color=ThemeColors.BG_CARD, align=PP_ALIGN.RIGHT)
        
        # GAP Vector Connector
        gap_box = slide.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(6.2), Inches(1.9), Inches(0.9), Inches(1.0))
        gap_box.fill.solid(); gap_box.fill.fore_color.rgb = ThemeColors.STATUS_RED; gap_box.line.fill.background()
        ShapeService.draw_textbox(slide, Inches(6.2), Inches(2.15), Inches(0.9), Inches(0.5), "فجوة\nتقنية", font_size=10, font_color=ThemeColors.WHITE, bold=True, align=PP_ALIGN.CENTER)
        
        # Low Level (Incomplete)
        ShapeService.draw_card(slide, Inches(7.3), Inches(1.5), Inches(5.5), Inches(1.8), ThemeColors.STATUS_RED, None)
        ShapeService.draw_textbox(slide, Inches(7.5), Inches(1.7), Inches(5.1), Inches(0.4), "المستوى المنخفض (غير مكتمل)", font_size=16, font_color=ThemeColors.WHITE, bold=True, align=PP_ALIGN.RIGHT)
        ShapeService.draw_textbox(slide, Inches(7.5), Inches(2.15), Inches(5.1), Inches(1.0), "مرحلة الترجمة المباشرة للغة الآلة والتحكم بالذاكرة (Assembly, Register Allocation).", font_size=11, font_color=ThemeColors.BG_CARD, align=PP_ALIGN.RIGHT)
        
        # Required list below
        ShapeService.draw_textbox(slide, Inches(0.5), Inches(3.6), Inches(12.333), Inches(0.4), "المكونات المطلوبة لسد الفجوة وبناء المترجم المستقل:", font_size=14, font_color=ThemeColors.PRIMARY, bold=True, align=PP_ALIGN.RIGHT)
        items = [
            "Semantic Analysis — فحص دلالات الأكواد والأنواع (Type Checking).",
            "Intermediate Representation (IR) — بناء طبقة وسيطة لفصل الكود عن المعالج.",
            "Backend Optimizer — تطبيق عمليات تحسين السجلات وتوزيع التعليمات البرمجية.",
            "Assembler — تحويل أكواد الـ Assembly المخرجة إلى لغة الآلة المباشرة (Binary Code)."
        ]
        ShapeService.draw_bullet_list(slide, Inches(0.5), Inches(4.2), Inches(12.333), Inches(2.5), items, font_size=12)

    @staticmethod
    def render_slide_20_roadmap_full(slide, sd, slide_idx):
        """Slide 20: Full 10-step conceptual developmental roadmap flow."""
        ShapeService.draw_header(slide, "خارطة طريق التطوير من ض العالية إلى لغة الآلة", slide_idx)
        
        # Top banner note
        note = sd.get("note", {}).get("text", "✓ موجود حالياً  ·  ○ مخطط للمستقبل")
        ShapeService.draw_card(slide, Inches(0.5), Inches(1.3), Inches(12.333), Inches(0.5), ThemeColors.BG_CARD, ThemeColors.PRIMARY)
        ShapeService.draw_textbox(slide, Inches(0.7), Inches(1.35), Inches(11.933), Inches(0.4), note, font_size=11, font_color=ThemeColors.PRIMARY, bold=True, align=PP_ALIGN.CENTER)
        
        flow = sd.get("flow", {})
        steps = flow.get("steps", [])
        
        col_w = Inches(2.2)
        row_h = Inches(1.8)
        gap_x = Inches(0.2)
        gap_y = Inches(0.2)
        start_x = Inches(0.5)
        start_y = Inches(2.1)
        
        for idx, step in enumerate(steps[:10]):
            c = idx % 5
            r = idx // 5
            x = start_x + c * (col_w + gap_x)
            y = start_y + r * (row_h + gap_y)
            
            bg = ThemeColors.PRIMARY if step.get("highlight") else ThemeColors.WHITE
            tx = ThemeColors.WHITE if step.get("highlight") else ThemeColors.TEXT_DARK
            border = ThemeColors.PRIMARY if step.get("highlight") else ThemeColors.BORDER
            
            ShapeService.draw_card(slide, x, y, col_w, row_h, bg, border, 1)
            ShapeService.draw_textbox(slide, x + Inches(0.1), y + Inches(0.5), col_w - Inches(0.2), Inches(0.8), step.get("text", ""), font_size=11, font_color=tx, bold=True, align=PP_ALIGN.CENTER)
            
            # Step numbering
            num_badge = slide.shapes.add_shape(MSO_SHAPE.OVAL, x + Inches(0.9), y + Inches(0.1), Inches(0.4), Inches(0.4))
            num_badge.fill.solid(); num_badge.fill.fore_color.rgb = ThemeColors.SECONDARY if step.get("highlight") else ThemeColors.BG_CARD
            num_badge.line.fill.background()
            ShapeService.draw_textbox(slide, x + Inches(0.9), y + Inches(0.12), Inches(0.4), Inches(0.35), f"{idx+1}", font_size=10, font_color=ThemeColors.WHITE if step.get("highlight") else ThemeColors.PRIMARY, bold=True, align=PP_ALIGN.CENTER)
