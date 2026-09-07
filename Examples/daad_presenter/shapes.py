# -*- coding: utf-8 -*-
"""
================================================================================
DAAD PRESENTATION ENGINE - SHAPE DRAWING & VECTOR SERVICES
================================================================================
Contains mathematical calculations, RTL wrappers, and shape builders.
================================================================================
"""

from pptx.util import Inches, Pt
from pptx.enum.shapes import MSO_SHAPE
from pptx.enum.text import PP_ALIGN
from daad_presenter.theme import ThemeColors, Typography, Dimensions

class ShapeService:
    @staticmethod
    def draw_card(slide, x, y, w, h, bg_color=ThemeColors.WHITE, border_color=ThemeColors.PRIMARY, border_width=1):
        """Draws a beautiful card container with smooth rounded corners."""
        card = slide.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, x, y, w, h)
        card.fill.solid()
        card.fill.fore_color.rgb = bg_color
        if border_color:
            card.line.color.rgb = border_color
            card.line.width = Pt(border_width)
        else:
            card.line.fill.background()
        return card

    @staticmethod
    def draw_textbox(slide, x, y, w, h, text, font_size=14, font_color=ThemeColors.TEXT_BODY, bold=False, align=PP_ALIGN.RIGHT, font_name=Typography.ARABIC):
        """Draws a word-wrapped text frame with precise alignment and style."""
        tx_box = slide.shapes.add_textbox(x, y, w, h)
        tf = tx_box.text_frame
        tf.word_wrap = True
        tf.margin_top = Inches(0.05)
        tf.margin_bottom = Inches(0.05)
        tf.margin_left = Inches(0.05)
        tf.margin_right = Inches(0.05)
        p = tf.paragraphs[0]
        p.text = text
        p.font.name = font_name
        p.font.size = Pt(font_size)
        p.font.bold = bold
        p.font.color.rgb = font_color
        p.alignment = align
        return tx_box

    @staticmethod
    def draw_bullet_list(slide, x, y, w, h, items, font_size=13, font_color=ThemeColors.TEXT_BODY):
        """Generates a perfectly aligned vertical list with custom clean bullet points."""
        tx_box = slide.shapes.add_textbox(x, y, w, h)
        tf = tx_box.text_frame
        tf.word_wrap = True
        tf.margin_top = Inches(0.05)
        tf.margin_bottom = Inches(0.05)
        for i, item in enumerate(items):
            p = tf.add_paragraph() if i > 0 else tf.paragraphs[0]
            p.text = f"  •  {item}"
            p.font.name = Typography.ARABIC
            p.font.size = Pt(font_size)
            p.font.color.rgb = font_color
            p.alignment = PP_ALIGN.RIGHT
            p.space_after = Pt(8)
        return tx_box

    @staticmethod
    def draw_header(slide, title, slide_num):
        """Draws the beautiful minimalist header with bottom separator line."""
        # Clean white background
        fill = slide.background.fill
        fill.solid()
        fill.fore_color.rgb = ThemeColors.WHITE
        
        # Bottom Separator Line
        line = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, Inches(0.5), Inches(0.95), Inches(12.333), Inches(0.015))
        line.fill.solid()
        line.fill.fore_color.rgb = ThemeColors.PRIMARY
        line.line.fill.background()
        
        # Logo Text (Top Left)
        ShapeService.draw_textbox(slide, Inches(0.5), Inches(0.15), Inches(3), Inches(0.4), "لغة ض", font_size=26, font_color=ThemeColors.PRIMARY, bold=True)
        ShapeService.draw_textbox(slide, Inches(0.5), Inches(0.55), Inches(3.5), Inches(0.3), "لغة برمجة عربية عالية المستوى", font_size=9, font_color=ThemeColors.TEXT_MUTED)
        
        # Decorative diamond accent beside the logo
        ShapeService.draw_diamond(slide, Inches(3.55), Inches(0.24), Inches(0.16), ThemeColors.ACCENT)
        ShapeService.draw_icon_badge(slide, Inches(3.9), Inches(0.2), Inches(0.3), inner="dot", fill_color=ThemeColors.BG_CARD, border_color=ThemeColors.PRIMARY)
        
        # Slide Number Box (Top Right - Floating style)
        num_w, num_h = Inches(0.7), Inches(0.42)
        num_x = Dimensions.SCREEN_W - Inches(0.5) - num_w
        num_y = Inches(0.28)
        ShapeService.draw_card(slide, num_x, num_y, num_w, num_h, ThemeColors.PRIMARY, border_color=None)
        ShapeService.draw_textbox(slide, num_x, num_y, num_w, num_h, f"{slide_num:02d}", font_size=15, font_color=ThemeColors.WHITE, bold=True, align=PP_ALIGN.CENTER)
        
        # Slide Title (Left of the number)
        title_w = Inches(6.5)
        title_x = num_x - title_w - Inches(0.2)
        ShapeService.draw_textbox(slide, title_x, num_y, title_w, num_h, title, font_size=24, font_color=ThemeColors.TEXT_DARK, bold=True, align=PP_ALIGN.LEFT)

    @staticmethod
    def draw_arrow(slide, x, y, w, h, color=ThemeColors.PRIMARY):
        """Draws a clean directional vector arrow."""
        arrow = slide.shapes.add_shape(MSO_SHAPE.RIGHT_ARROW, x, y, w, h)
        arrow.fill.solid()
        arrow.fill.fore_color.rgb = color
        arrow.line.fill.background()
        return arrow

    @staticmethod
    def draw_diamond(slide, x, y, size, color=ThemeColors.PRIMARY):
        """Draws a rotated-square diamond accent."""
        sq = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, x, y, size, size)
        sq.fill.solid()
        sq.fill.fore_color.rgb = color
        sq.line.fill.background()
        sq.rotation = 45
        return sq

    @staticmethod
    def draw_plus(slide, x, y, size, color=ThemeColors.ACCENT, thick=0.06):
        """Draws a clean vector plus sign from two bars."""
        w = Inches(thick)
        b1 = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, x + size/2 - w/2, y, w, size)
        b1.fill.solid(); b1.fill.fore_color.rgb = color; b1.line.fill.background()
        b2 = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, x, y + size/2 - w/2, size, w)
        b2.fill.solid(); b2.fill.fore_color.rgb = color; b2.line.fill.background()

    @staticmethod
    def draw_icon_badge(slide, x, y, size, inner="diamond", fill_color=ThemeColors.BG_CARD, border_color=ThemeColors.PRIMARY):
        """Draws a circular badge with an inner geometric icon inside."""
        circ = slide.shapes.add_shape(MSO_SHAPE.OVAL, x, y, size, size)
        circ.fill.solid()
        circ.fill.fore_color.rgb = fill_color
        circ.line.color.rgb = border_color
        circ.line.width = Pt(1.5)
        inner_size = int(size/3.2)
        cx = x + size/2 - inner_size/2
        cy = y + size/2 - inner_size/2
        if inner == "diamond":
            ShapeService.draw_diamond(slide, cx, cy, inner_size, border_color)
        elif inner == "plus":
            ShapeService.draw_plus(slide, cx, cy, inner_size, border_color)
        elif inner == "dot":
            dot = slide.shapes.add_shape(MSO_SHAPE.OVAL, cx, cy, inner_size, inner_size)
            dot.fill.solid(); dot.fill.fore_color.rgb = border_color; dot.line.fill.background()
        elif inner == "square":
            sq = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, cx, cy, inner_size, inner_size)
            sq.fill.solid(); sq.fill.fore_color.rgb = border_color; sq.line.fill.background()
        return circ

    @staticmethod
    def decorate_slide(slide, slide_idx):
        """Adds ambient vector drawings to every slide: corner motifs, edge nodes, and a bottom data band."""
        PRI = ThemeColors.PRIMARY
        ACC = ThemeColors.ACCENT
        SEC = ThemeColors.SECONDARY
        MUT = ThemeColors.TEXT_MUTED
        BG = ThemeColors.BG_CARD

        # --- Corner Motifs ---
        ShapeService.draw_diamond(slide, Inches(0.14), Inches(0.12), Inches(0.13), PRI)
        ShapeService.draw_diamond(slide, Inches(0.34), Inches(0.36), Inches(0.08), ACC)
        tri = slide.shapes.add_shape(MSO_SHAPE.ISOSCELES_TRIANGLE, Inches(12.94), Inches(0.28), Inches(0.30), Inches(0.20))
        tri.fill.solid(); tri.fill.fore_color.rgb = BG
        tri.line.color.rgb = PRI; tri.line.width = Pt(1)

        # --- Edge Node Dots (right edge) ---
        for i, yy in enumerate([2.4, 3.6, 4.8]):
            dot = slide.shapes.add_shape(MSO_SHAPE.OVAL, Inches(13.02), Inches(yy), Inches(0.09), Inches(0.09))
            dot.fill.solid(); dot.fill.fore_color.rgb = ACC if i % 2 == 0 else PRI
            dot.line.fill.background()

        # --- Edge Node Dots (left edge) ---
        for i, yy in enumerate([2.8, 4.2]):
            dot = slide.shapes.add_shape(MSO_SHAPE.OVAL, Inches(0.16), Inches(yy), Inches(0.09), Inches(0.09))
            dot.fill.solid(); dot.fill.fore_color.rgb = SEC if i == 0 else MUT
            dot.line.fill.background()

        # --- Bottom Data Band (safe zone y 7.18 - 7.46) ---
        band_y = Inches(7.30)
        base = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, Inches(0.5), band_y, Inches(12.333), Inches(0.02))
        base.fill.solid(); base.fill.fore_color.rgb = MUT; base.line.fill.background()

        # left: ascending mini bars
        for i, h in enumerate([Inches(0.10), Inches(0.15), Inches(0.20)]):
            bar = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, Inches(0.62 + i*0.15), band_y - h, Inches(0.10), h)
            bar.fill.solid(); bar.fill.fore_color.rgb = PRI if i == 2 else ACC
            bar.line.fill.background()

        # center: node graph connector + dots
        conn = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, Inches(5.0), Inches(7.315), Inches(1.4), Inches(0.02))
        conn.fill.solid(); conn.fill.fore_color.rgb = MUT; conn.line.fill.background()
        for i, xx in enumerate([Inches(5.1), Inches(5.7), Inches(6.3)]):
            nd = slide.shapes.add_shape(MSO_SHAPE.OVAL, xx, Inches(7.24), Inches(0.16), Inches(0.16))
            nd.fill.solid(); nd.fill.fore_color.rgb = SEC if i == 1 else PRI
            nd.line.fill.background()

        # right: barcode squares + diamond
        for i in range(4):
            sq = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, Inches(11.6 + i*0.17), Inches(7.30), Inches(0.10), Inches(0.12 + 0.02*i))
            sq.fill.solid(); sq.fill.fore_color.rgb = PRI if i % 2 == 0 else ACC
            sq.line.fill.background()
        ShapeService.draw_diamond(slide, Inches(12.5), Inches(7.22), Inches(0.14), SEC)

    @staticmethod
    def draw_ide_block(slide, x, y, w, h, title, code):
        """Renders a fully customized IDE code editor block with line numbering."""
        ShapeService.draw_card(slide, x, y, w, h, ThemeColors.CODE_BG, ThemeColors.BORDER, 0.5)
        
        # Header Area
        header_h = Inches(0.4)
        bar = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, x, y, w, header_h)
        bar.fill.solid()
        bar.fill.fore_color.rgb = ThemeColors.PRIMARY
        bar.line.fill.background()
        
        # Header Title
        ShapeService.draw_textbox(slide, x, y, w, header_h, title, font_size=11, font_color=ThemeColors.WHITE, bold=True, align=PP_ALIGN.CENTER)
        
        # IDE Buttons (Visual cue mimicking real IDE)
        for i, color in enumerate([ThemeColors.STATUS_RED, ThemeColors.STATUS_ORANGE, ThemeColors.PRIMARY]):
            btn = slide.shapes.add_shape(MSO_SHAPE.OVAL, x + Inches(0.15 + i*0.2), y + Inches(0.12), Inches(0.15), Inches(0.15))
            btn.fill.solid(); btn.fill.fore_color.rgb = color; btn.line.fill.background()
            
        # Lines Container
        lines_box = slide.shapes.add_textbox(x + Inches(0.15), y + header_h + Inches(0.1), w - Inches(0.3), h - header_h - Inches(0.2))
        tf = lines_box.text_frame
        tf.word_wrap = True
        
        for i, line in enumerate(code.split('\n')):
            p = tf.add_paragraph() if i > 0 else tf.paragraphs[0]
            p.text = f"{i+1:2d}   {line}"
            p.font.name = Typography.CODE
            p.font.size = Pt(13)
            p.font.color.rgb = ThemeColors.SECONDARY
            p.alignment = PP_ALIGN.LEFT
