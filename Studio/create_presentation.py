"""
Create professional PowerPoint presentation about Daad language
Based on PROJECT_LANGUAGE_REPORT.md - Black/White/Glass Green theme
"""

from pptx import Presentation
from pptx.util import Inches, Pt, Emu
from pptx.dml.color import RGBColor
from pptx.enum.text import PP_ALIGN, MSO_ANCHOR
from pptx.enum.shapes import MSO_SHAPE
import os

# â”€â”€ Professional Color Palette â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
BLACK = RGBColor(0x0A, 0x0A, 0x0A)
DARK_BG = RGBColor(0x11, 0x11, 0x11)
CARD_BG = RGBColor(0x1A, 0x1A, 0x1A)
CARD_BORDER = RGBColor(0x2A, 0x2A, 0x2A)
WHITE = RGBColor(0xFF, 0xFF, 0xFF)
OFF_WHITE = RGBColor(0xF0, 0xF0, 0xF0)
LIGHT_GRAY = RGBColor(0xBB, 0xBB, 0xBB)
MID_GRAY = RGBColor(0x77, 0x77, 0x77)
DARK_GRAY = RGBColor(0x33, 0x33, 0x33)
# Glass Green - Primary Accent
GLASS_GREEN = RGBColor(0x00, 0xC9, 0xA7)
GLASS_GREEN_LIGHT = RGBColor(0x00, 0xE6, 0xC3)
GLASS_GREEN_DARK = RGBColor(0x00, 0x9B, 0x82)
# Subtle accents
MINT_BG = RGBColor(0xE8, 0xF8, 0xF5)
MINT_LIGHT = RGBColor(0xF0, 0xFA, 0xF7)

# â”€â”€ Helpers â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

def add_bg(slide, color=DARK_BG):
    bg = slide.background
    fill = bg.fill
    fill.solid()
    fill.fore_color.rgb = color

def add_rect(slide, left, top, width, height, color):
    shape = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, left, top, width, height)
    shape.fill.solid()
    shape.fill.fore_color.rgb = color
    shape.line.fill.background()
    return shape

def add_rounded_rect(slide, left, top, width, height, color):
    shape = slide.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, left, top, width, height)
    shape.fill.solid()
    shape.fill.fore_color.rgb = color
    shape.line.fill.background()
    return shape

def add_text_box(slide, left, top, width, height, text, font_size=18,
                 color=WHITE, bold=False, alignment=PP_ALIGN.RIGHT,
                 font_name="Segoe UI"):
    txBox = slide.shapes.add_textbox(left, top, width, height)
    tf = txBox.text_frame
    tf.word_wrap = True
    p = tf.paragraphs[0]
    p.text = text
    p.font.size = Pt(font_size)
    p.font.color.rgb = color
    p.font.bold = bold
    p.font.name = font_name
    p.alignment = alignment
    p.space_after = Pt(2)
    return txBox

def add_multiline_box(slide, left, top, width, height, lines, font_size=16,
                      color=WHITE, alignment=PP_ALIGN.RIGHT, font_name="Segoe UI"):
    txBox = slide.shapes.add_textbox(left, top, width, height)
    tf = txBox.text_frame
    tf.word_wrap = True
    for i, line_data in enumerate(lines):
        p = tf.paragraphs[0] if i == 0 else tf.add_paragraph()
        if isinstance(line_data, dict):
            p.text = line_data.get("text", "")
            p.font.size = Pt(line_data.get("size", font_size))
            p.font.color.rgb = line_data.get("color", color)
            p.font.bold = line_data.get("bold", False)
            p.font.name = line_data.get("font", font_name)
            p.alignment = line_data.get("align", alignment)
            p.space_after = Pt(line_data.get("space_after", 4))
        else:
            p.text = line_data
            p.font.size = Pt(font_size)
            p.font.color.rgb = color
            p.font.name = font_name
            p.alignment = alignment
            p.space_after = Pt(4)
    return txBox

def add_code_box(slide, left, top, width, height, code, font_size=13):
    shape = add_rect(slide, left, top, width, height, CARD_BG)
    shape.line.color.rgb = CARD_BORDER
    shape.line.width = Pt(1)
    txBox = slide.shapes.add_textbox(left + Inches(0.25), top + Inches(0.2),
                                      width - Inches(0.5), height - Inches(0.4))
    tf = txBox.text_frame
    tf.word_wrap = True
    for i, line in enumerate(code.split("\n")):
        p = tf.paragraphs[0] if i == 0 else tf.add_paragraph()
        p.text = line
        p.font.size = Pt(font_size)
        p.font.color.rgb = GLASS_GREEN
        p.font.name = "Consolas"
        p.alignment = PP_ALIGN.LEFT
        p.space_after = Pt(2)
    return shape

def add_arrow(slide, left, top, width, height, color=GLASS_GREEN):
    shape = slide.shapes.add_shape(MSO_SHAPE.RIGHT_ARROW, left, top, width, height)
    shape.fill.solid()
    shape.fill.fore_color.rgb = color
    shape.line.fill.background()
    return shape

def add_box(slide, left, top, width, height, text, bg_color=GLASS_GREEN,
            text_color=WHITE, font_size=14, bold=True, font_name="Segoe UI"):
    shape = slide.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, left, top, width, height)
    shape.fill.solid()
    shape.fill.fore_color.rgb = bg_color
    shape.line.fill.background()
    tf = shape.text_frame
    tf.word_wrap = True
    tf.paragraphs[0].alignment = PP_ALIGN.CENTER
    p = tf.paragraphs[0]
    p.text = text
    p.font.size = Pt(font_size)
    p.font.color.rgb = text_color
    p.font.bold = bold
    p.font.name = font_name
    return shape

def add_circle(slide, left, top, size, color):
    shape = slide.shapes.add_shape(MSO_SHAPE.OVAL, left, top, size, size)
    shape.fill.solid()
    shape.fill.fore_color.rgb = color
    shape.line.fill.background()
    return shape

def add_bullet_list(slide, left, top, width, height, items, font_size=15,
                    color=LIGHT_GRAY, bullet_color=GLASS_GREEN):
    txBox = slide.shapes.add_textbox(left, top, width, height)
    tf = txBox.text_frame
    tf.word_wrap = True
    for i, item in enumerate(items):
        p = tf.paragraphs[0] if i == 0 else tf.add_paragraph()
        p.text = f"  {item}"
        p.font.size = Pt(font_size)
        p.font.color.rgb = color
        p.font.name = "Segoe UI"
        p.alignment = PP_ALIGN.RIGHT
        p.space_after = Pt(8)
    return txBox

def add_section_line(slide, left, top, width, color=GLASS_GREEN):
    shape = add_rect(slide, left, top, width, Inches(0.02), color)
    return shape

def add_slide_number(slide, num, total):
    add_text_box(slide, Inches(6), Inches(7.0), Inches(1.3), Inches(0.4),
                 f"{num} / {total}", font_size=10, color=MID_GRAY,
                 alignment=PP_ALIGN.CENTER, font_name="Segoe UI Light")

def add_top_bar(slide):
    add_rect(slide, Inches(0), Inches(0), Inches(13.333), Inches(0.06), GLASS_GREEN)

TOTAL_SLIDES = 29

# â”€â”€ Create Presentation â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

prs = Presentation()
prs.slide_width = Inches(13.333)
prs.slide_height = Inches(7.5)

W = prs.slide_width
H = prs.slide_height

# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
# Slide 1: Cover
# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
slide = prs.slides.add_slide(prs.slide_layouts[6])
add_bg(slide)

# Top accent bar
add_rect(slide, Inches(0), Inches(0), W, Inches(0.08), GLASS_GREEN)

# Centered content
add_text_box(slide, Inches(1), Inches(2.2), Inches(11.3), Inches(1.5),
             "ظ„ط؛ط© آ«ط¶آ»", font_size=80, color=WHITE, bold=True,
             alignment=PP_ALIGN.CENTER, font_name="Segoe UI Light")

add_text_box(slide, Inches(1), Inches(3.8), Inches(11.3), Inches(0.6),
             "ظ„ط؛ط© ط¨ط±ظ…ط¬ط© ط¹ط±ط¨ظٹط© ط¹ط§ظ„ظٹط© ط§ظ„ظ…ط³طھظˆظ‰", font_size=24, color=GLASS_GREEN,
             alignment=PP_ALIGN.CENTER)

add_section_line(slide, Inches(5.5), Inches(4.7), Inches(2.3))

add_text_box(slide, Inches(1), Inches(5.2), Inches(11.3), Inches(0.5),
             "Dhad Studio  آ·  ط§ظ„ط¥طµط¯ط§ط± 1.0.0  آ·  MIT License", font_size=14,
             color=MID_GRAY, alignment=PP_ALIGN.CENTER)

add_text_box(slide, Inches(1), Inches(5.8), Inches(11.3), Inches(0.5),
             "ط§ظ„ظ…ط·ظˆط±: ظ…ط­ظ…ط¯ ظ…ط­ظ…ظˆط¯ ط§ظ„ط­ظ…ظˆط²", font_size=14, color=LIGHT_GRAY,
             alignment=PP_ALIGN.CENTER)

add_slide_number(slide, 1, TOTAL_SLIDES)

# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
# Slide 2: What is Daad?
# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
slide = prs.slides.add_slide(prs.slide_layouts[6])
add_bg(slide)
add_top_bar(slide)

add_text_box(slide, Inches(0.8), Inches(0.5), Inches(11.7), Inches(0.8),
             "ظ…ط§ ظ‡ظٹ ظ„ط؛ط© آ«ط¶آ»طں", font_size=36, color=WHITE, bold=True,
             alignment=PP_ALIGN.RIGHT)

add_section_line(slide, Inches(8.5), Inches(1.2), Inches(4))

cards = [
    ("ظ„ط؛ط© ط¨ط±ظ…ط¬ط©", "ط¹ط±ط¨ظٹط© ط¨ط§ظ„ظƒط§ظ…ظ„\nظƒظ„ظ…ط§طھ ظ…ظپطھط§ط­ظٹط© ط¹ط±ط¨ظٹط©\nطھط¹ظ„ظٹظ‚ط§طھ ط¹ط±ط¨ظٹط©\nط£ط³ظ…ط§ط، ظ…طھط؛ظٹط±ط§طھ ط¹ط±ط¨ظٹط©"),
    ("ط¹ط§ظ„ظٹط© ط§ظ„ظ…ط³طھظˆظ‰", "ظ…ط¨ظ†ظٹط© ط¹ظ„ظ‰ C++20\nطھط³طھط®ظ„طµ ط§ظ„طھط¹ظ‚ظٹط¯\nطھط³ظٹظ‘ط± ط§ظ„ط¨ط±ظ…ط¬ط©\nطھظڈط±ظƒظ‘ط² ط¹ظ„ظ‰ ط§ظ„ظ…ظ†ط·ظ‚"),
    ("ظ„طھط¹ظ„ظٹظ… ط§ظ„ط¨ط±ظ…ط¬ط©", "ظ…طµظ…ظ…ط© ظ„ظ„ظ…طھط¹ظ„ظ…ظٹظ†\nط¨ظٹط¦ط© طھظپط§ط¹ظ„ظٹط©\nط£ظ„ط¹ط§ط¨ ظˆط£ظ…ط«ظ„ط©\nظ…ظ†طµط© طھط¹ظ„ظٹظ…ظٹط©"),
    ("Transpiler", "طھطھط±ط¬ظ… ط¥ظ„ظ‰ C++20\nط£ظˆ JavaScript\nظ„ظٹط³طھ Interpreter\nظˆظ„ظٹط³طھ Compiler ظ…ط¨ط§ط´ط±"),
]

for i, (title, desc) in enumerate(cards):
    x = Inches(0.5 + i * 3.15)
    y = Inches(1.8)
    card = add_rounded_rect(slide, x, y, Inches(2.9), Inches(4.5), CARD_BG)
    card.line.color.rgb = CARD_BORDER
    card.line.width = Pt(1)
    # Green top line on card
    add_rect(slide, x + Inches(0.3), y + Inches(0.25), Inches(2.3), Inches(0.03), GLASS_GREEN)
    add_text_box(slide, x + Inches(0.3), y + Inches(0.5), Inches(2.3), Inches(0.5),
                 title, font_size=18, color=GLASS_GREEN, bold=True, alignment=PP_ALIGN.CENTER)
    add_multiline_box(slide, x + Inches(0.3), y + Inches(1.3), Inches(2.3), Inches(3),
                      desc.split("\n"), font_size=14, color=LIGHT_GRAY,
                      alignment=PP_ALIGN.CENTER)

add_slide_number(slide, 2, TOTAL_SLIDES)

# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
# Slide 3: Why Daad?
# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
slide = prs.slides.add_slide(prs.slide_layouts[6])
add_bg(slide)
add_top_bar(slide)

add_text_box(slide, Inches(0.8), Inches(0.5), Inches(11.7), Inches(0.8),
             "ظ„ظ…ط§ط°ط§ آ«ط¶آ»طں", font_size=36, color=WHITE, bold=True,
             alignment=PP_ALIGN.RIGHT)

add_section_line(slide, Inches(9), Inches(1.2), Inches(3.5))

problems = [
    ("?", "ط§ظ„ظپط¬ظˆط© ط§ظ„ظ„ط؛ظˆظٹط©",
     "ظ…ط¹ط¸ظ… ظ„ط؛ط§طھ ط§ظ„ط¨ط±ظ…ط¬ط©\nطھط³طھط®ط¯ظ… ط§ظ„ط¥ظ†ط¬ظ„ظٹط²ظٹط©\nظƒط¹ط§ط¦ظ‚ ط£ظ…ط§ظ… ط§ظ„ظ…طھط¹ظ„ظ…ظٹظ† ط§ظ„ط¹ط±ط¨"),
    ("+", "ط§ظ„ط­ظ„",
     "ظ„ط؛ط© آ«ط¶آ» طھط²ظٹظ„ ط§ظ„ط­ظˆط§ط¬ط²\nطھطھظٹط­ ط§ظ„ط¨ط±ظ…ط¬ط© ط¨ط§ظ„ط¹ط±ط¨ظٹط©\nط¨ظƒظ„ظ…ط§طھ ظ…ظپظ‡ظˆظ…ط© ظˆظ…ط£ظ„ظˆظپط©"),
    ("*", "ط§ظ„ظ†طھظٹط¬ط©",
     "طھط¹ظ„ظ… ط£ط³ط±ط¹\nظپظ‡ظ… ط£ط¹ظ…ظ‚\nظ…ط´ط§ط±ظƒط© ط§ظ„ظ…ط¹ط±ظپط©\nط¨ط§ظ„ظ„ط؛ط© ط§ظ„ط¹ط±ط¨ظٹط©"),
]

colors_list = [RGBColor(0xFF, 0x55, 0x55), GLASS_GREEN, RGBColor(0x55, 0xFF, 0xAA)]

for i, (icon, title, desc) in enumerate(problems):
    x = Inches(0.8 + i * 4.1)
    y = Inches(2.0)
    # Icon circle
    circle = add_circle(slide, x + Inches(1.2), y, Inches(0.9), colors_list[i])
    add_text_box(slide, x + Inches(1.2), y + Inches(0.12), Inches(0.9), Inches(0.65),
                 icon, font_size=32, color=WHITE, bold=True, alignment=PP_ALIGN.CENTER)
    add_text_box(slide, x, y + Inches(1.2), Inches(3.5), Inches(0.5),
                 title, font_size=20, color=colors_list[i], bold=True, alignment=PP_ALIGN.CENTER)
    add_multiline_box(slide, x, y + Inches(1.9), Inches(3.5), Inches(2.5),
                      desc.split("\n"), font_size=14, color=LIGHT_GRAY,
                      alignment=PP_ALIGN.CENTER)

add_text_box(slide, Inches(0.5), Inches(6.2), Inches(12.3), Inches(0.5),
             "آ«ط¶ â€” ط¨ط±ظ…ط¬ط© ط¨ظ„ط§ ط­ظˆط§ط¬ط² ظ„ط؛ظˆظٹط©آ»", font_size=16, color=MID_GRAY,
             alignment=PP_ALIGN.CENTER)

add_slide_number(slide, 3, TOTAL_SLIDES)

# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
# Slide 4: Target Audience
# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
slide = prs.slides.add_slide(prs.slide_layouts[6])
add_bg(slide)
add_top_bar(slide)

add_text_box(slide, Inches(0.8), Inches(0.5), Inches(11.7), Inches(0.8),
             "ظ„ظ…ظ† طµظڈظ…ظ…طھطں", font_size=36, color=WHITE, bold=True,
             alignment=PP_ALIGN.RIGHT)

add_section_line(slide, Inches(9.5), Inches(1.2), Inches(3))

audiences = [
    ("01", "ط§ظ„ط·ظ„ط§ط¨ ظˆط§ظ„ظ…طھط¹ظ„ظ…ظٹظ†", "ط§ظ„ط°ظٹظ† ظٹط±ط؛ط¨ظˆظ† ظپظٹ طھط¹ظ„ظ…\nط§ظ„ط¨ط±ظ…ط¬ط© ط¨ظ„ط؛طھظ‡ظ… ط§ظ„ط£ظ…"),
    ("02", "ط§ظ„ظ…ط¹ظ„ظ…ظˆظ†", "ط§ظ„ط°ظٹظ† ظٹط­طھط§ط¬ظˆظ† ط£ط¯ط§ط©\nطھط¹ظ„ظٹظ…ظٹط© ط¹ط±ط¨ظٹط©"),
    ("03", "ط§ظ„ظ…ط¨ط±ظ…ط¬ظˆظ† ط§ظ„ظ…ط¨طھط¯ط¦ظˆظ†", "ط§ظ„ط°ظٹظ† ظٹظپط¶ظ„ظˆظ† ط§ظ„طھط¹ظ„ظ…\nط¨ط§ظ„ط¹ط±ط¨ظٹط© ط£ظˆظ„ط§ظ‹"),
    ("04", "ظ…ظ†ط´ط¦ظˆ ط§ظ„ظ…ط­طھظˆظ‰", "ظ„ط¥ظ†طھط§ط¬ ط¯ظˆط±ط§طھ\nظˆظ…ظˆط§ط¯ طھط¹ظ„ظٹظ…ظٹط©"),
]

for i, (num, title, desc) in enumerate(audiences):
    x = Inches(0.5 + i * 3.15)
    y = Inches(1.8)
    card = add_rounded_rect(slide, x, y, Inches(2.9), Inches(3.8), CARD_BG)
    card.line.color.rgb = CARD_BORDER
    card.line.width = Pt(1)
    add_text_box(slide, x + Inches(0.3), y + Inches(0.3), Inches(2.3), Inches(0.8),
                 num, font_size=36, color=GLASS_GREEN, bold=True, alignment=PP_ALIGN.CENTER,
                 font_name="Segoe UI Light")
    add_text_box(slide, x + Inches(0.3), y + Inches(1.4), Inches(2.3), Inches(0.5),
                 title, font_size=18, color=WHITE, bold=True, alignment=PP_ALIGN.CENTER)
    add_multiline_box(slide, x + Inches(0.3), y + Inches(2.2), Inches(2.3), Inches(1.2),
                      desc.split("\n"), font_size=13, color=LIGHT_GRAY,
                      alignment=PP_ALIGN.CENTER)

add_slide_number(slide, 4, TOTAL_SLIDES)

# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
# Slide 5: Design Philosophy
# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
slide = prs.slides.add_slide(prs.slide_layouts[6])
add_bg(slide)
add_top_bar(slide)

add_text_box(slide, Inches(0.8), Inches(0.5), Inches(11.7), Inches(0.8),
             "ظپظ„ط³ظپط© ط§ظ„طھطµظ…ظٹظ…", font_size=36, color=WHITE, bold=True,
             alignment=PP_ALIGN.RIGHT)

add_section_line(slide, Inches(8), Inches(1.2), Inches(4.5))

principles = [
    ("ط¹ط±ط¨ظٹط© ط¨ط§ظ„ظƒط§ظ…ظ„", "ظƒظ„ ظƒظ„ظ…ط§طھ ط§ظ„ظ…ظپطھط§ط­ظٹط©\nظˆط§ظ„طھط¹ظ„ظٹظ‚ط§طھ ط¨ط§ظ„ط¹ط±ط¨ظٹط©"),
    ("ط³ظ‡ظˆظ„ط© ط§ظ„طھط¹ظ„ظ…", "syntax ط¨ط³ظٹط· ظˆظˆط§ط¶ط­\nظٹظڈط³ظ‡ظ‘ظ„ ط§ظ„ط¯ط®ظˆظ„ ظ„ظ„ط¨ط±ظ…ط¬ط©"),
    ("ط§ظ„ظˆط¶ظˆط­", "ظƒظˆط¯ ظ…ظ‚ط±ظˆط، ظˆظ…ظپظ‡ظˆظ…\nط­طھظ‰ ظ„ظ„ظ…ط¨طھط¯ط¦ظٹظ†"),
    ("ظ‚ط§ط¨ظ„ظٹط© ط§ظ„طھظˆط³ط¹", "طھطµظ…ظٹظ… modular\nظٹط³ظ…ط­ ط¨ط¥ط¶ط§ظپط© ظ…ظٹط²ط§طھ"),
]

for i, (title, desc) in enumerate(principles):
    x = Inches(0.5 + i * 3.15)
    y = Inches(1.8)
    card = add_rounded_rect(slide, x, y, Inches(2.9), Inches(4), CARD_BG)
    card.line.color.rgb = CARD_BORDER
    card.line.width = Pt(1)
    add_rect(slide, x, y, Inches(2.9), Inches(0.04), GLASS_GREEN)
    add_text_box(slide, x + Inches(0.3), y + Inches(0.4), Inches(2.3), Inches(0.5),
                 title, font_size=18, color=GLASS_GREEN, bold=True, alignment=PP_ALIGN.CENTER)
    add_multiline_box(slide, x + Inches(0.3), y + Inches(1.2), Inches(2.3), Inches(2.2),
                      desc.split("\n"), font_size=14, color=LIGHT_GRAY,
                      alignment=PP_ALIGN.CENTER)

add_slide_number(slide, 5, TOTAL_SLIDES)

# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
# Slide 6: Real Code Example
# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
slide = prs.slides.add_slide(prs.slide_layouts[6])
add_bg(slide)
add_top_bar(slide)

add_text_box(slide, Inches(0.8), Inches(0.5), Inches(11.7), Inches(0.8),
             "ظ…ط«ط§ظ„ ط­ظ‚ظٹظ‚ظٹ ط¨ظ„ط؛ط© آ«ط¶آ»", font_size=36, color=WHITE, bold=True,
             alignment=PP_ALIGN.RIGHT)

add_section_line(slide, Inches(7.5), Inches(1.2), Inches(5))

code = """// ط­ط§ط³ط¨ط© ط¨ظ„ط؛ط© ط¶
ط¯ط§ظ„ط© ط¬ظ…ط¹( طµط­ظٹط­ ط£, طµط­ظٹط­ ط¨ ) -> طµط­ظٹط­ {
    ط§ط±ط¬ط¹ ط£ + ط¨ ط›
}

طµط­ظٹط­ ط§ظ„ظ†طھظٹط¬ط© = ط¬ظ…ط¹( 10, 5 ) ط›
ط·ط¨ط§ط¹ط©( "ط§ظ„ظ…ط¬ظ…ظˆط¹ = ", ط§ظ„ظ†طھظٹط¬ط© ) ط›"""

add_code_box(slide, Inches(0.8), Inches(1.8), Inches(7), Inches(4.5), code, font_size=16)

annotations = [
    (Inches(8.5), Inches(2.2), "طھط¹ط±ظٹظپ ط¯ط§ظ„ط©", "ط¨ظƒظ„ظ…ط© آ«ط¯ط§ظ„ط©آ» + ط§ظ„ظ…ط¹ط§ظ…ظ„ط§طھ"),
    (Inches(8.5), Inches(3.4), "ظ†ظˆط¹ ط§ظ„ط¥ط±ط¬ط§ط¹", "طµط­ظٹط­ â†’ int"),
    (Inches(8.5), Inches(4.6), "ط·ط¨ط§ط¹ط©", "ط¯ط§ظ„ط© ط§ظ„ط·ط¨ط§ط¹ط© ط§ظ„ط¹ط±ط¨ظٹط©"),
]

for x, y, title, desc in annotations:
    card = add_rounded_rect(slide, x, y, Inches(4), Inches(0.9), CARD_BG)
    card.line.color.rgb = CARD_BORDER
    card.line.width = Pt(1)
    add_rect(slide, x, y, Inches(0.05), Inches(0.9), GLASS_GREEN)
    add_text_box(slide, x + Inches(0.25), y + Inches(0.08), Inches(3.5), Inches(0.35),
                 title, font_size=14, color=GLASS_GREEN, bold=True, alignment=PP_ALIGN.RIGHT)
    add_text_box(slide, x + Inches(0.25), y + Inches(0.45), Inches(3.5), Inches(0.35),
                 desc, font_size=12, color=LIGHT_GRAY, alignment=PP_ALIGN.RIGHT)

add_slide_number(slide, 6, TOTAL_SLIDES)

# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
# Slide 7: Language Syntax Quick Look
# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
slide = prs.slides.add_slide(prs.slide_layouts[6])
add_bg(slide)
add_top_bar(slide)

add_text_box(slide, Inches(0.8), Inches(0.5), Inches(11.7), Inches(0.8),
             "ظƒظٹظپ طھط¨ط¯ظˆ ظ„ط؛ط© آ«ط¶آ»طں", font_size=36, color=WHITE, bold=True,
             alignment=PP_ALIGN.RIGHT)

add_section_line(slide, Inches(7.5), Inches(1.2), Inches(5))

examples = [
    ("ظ…طھط؛ظٹط±", "طµط­ظٹط­ ط³ = 10 ط›"),
    ("ط´ط±ط·", "ط¥ط°ط§ ( ط³ > 5 ) {\n    ط·ط¨ط§ط¹ط©( \u201cط£ظƒط¨ط±\u201d ) ط›\n}"),
    ("ط­ظ„ظ‚ط©", "ظ„ظƒظ„ ( ط¹ = 0 ط› ط¹ < 5 ط› ط¹ = ط¹ + 1 ) {\n    ط·ط¨ط§ط¹ط©( ط¹ ) ط›\n}"),
    ("ط¯ط§ظ„ط©", "طµط­ظٹط­ ظ…ط¬ظ…ظˆط¹( طµط­ظٹط­ ط£ طŒ طµط­ظٹط­ ط¨ ) {\n    ط§ط±ط¬ط¹ ط£ + ط¨ ط›\n}"),
]

for i, (label, code) in enumerate(examples):
    y = Inches(1.6 + i * 1.4)
    add_box(slide, Inches(9.5), y, Inches(2.5), Inches(0.4), label,
            bg_color=GLASS_GREEN, text_color=WHITE, font_size=13)
    add_code_box(slide, Inches(0.5), y, Inches(8.5), Inches(1.15), code, font_size=13)

add_slide_number(slide, 7, TOTAL_SLIDES)

# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
# Slide 8: Language Components
# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
slide = prs.slides.add_slide(prs.slide_layouts[6])
add_bg(slide)
add_top_bar(slide)

add_text_box(slide, Inches(0.8), Inches(0.5), Inches(11.7), Inches(0.8),
             "ظ…ظƒظˆظ†ط§طھ ط§ظ„ظ„ط؛ط©", font_size=36, color=WHITE, bold=True,
             alignment=PP_ALIGN.RIGHT)

add_section_line(slide, Inches(8.5), Inches(1.2), Inches(4))

components = [
    ("ط§ظ„ظ…طھط؛ظٹط±ط§طھ", "طµط­ظٹط­طŒ ط¹ط´ط±ظٹطŒ ظ†طµ\nظ…ظ†ط·ظ‚ظٹطŒ ط­ط±ظپطŒ ظپط±ط§ط؛"),
    ("ط§ظ„ط¹ظ…ظ„ظٹط§طھ", "+طŒ -طŒ *طŒ /طŒ %طŒ ^\n==طŒ !=طŒ <طŒ >طŒ &&طŒ ||"),
    ("ط§ظ„طھط­ظƒظ…", "ط¥ط°ط§/ظˆط¥ظ„ط§\nط·ط§ظ„ظ…ط§/ظ„ظƒظ„/ط§ظپط¹ظ„\nط§ط®طھط±/ط­ط§ظ„ط©"),
    ("ط§ظ„ط¯ظˆط§ظ„", "طھط¹ط±ظٹظپ ظˆط§ط³طھط¯ط¹ط§ط،\nظ†ظˆط¹ ط¥ط±ط¬ط§ط¹\nظ…ط¹ط§ظ…ظ„ط§طھ"),
    ("ط§ظ„ظ…طµظپظˆظپط§طھ", "طµط­ظٹط­[]\nط¹ط´ط±ظٹ[]\nظ†طµ[]"),
    ("OOP", "طµظ†ظپ/ظپط¦ط©\nظ‡ظٹظƒظ„\nطھط¹ط¯ط§ط¯"),
    ("ظ…ط¹ط§ظ„ط¬ط© ط§ظ„ط£ط®ط·ط§ط،", "ط­ط§ظˆظ„/ط§ظ…ط³ظƒ\nط£ط®ظٹط±ط§ظ‹\nط§ط±ظ…ظگ"),
    ("ط§ظ„ظ…ظƒطھط¨ط§طھ", "ظ†طµظˆطµطŒ ط±ظٹط§ط¶ظٹط§طھ\nط´ط¨ظƒط§طھطŒ ط±ط³ظˆظ…ظٹط§طھ\n+200 ط¯ط§ظ„ط©"),
]

for i, (title, desc) in enumerate(components):
    col = i % 4
    row = i // 4
    x = Inches(0.5 + col * 3.15)
    y = Inches(1.6 + row * 2.8)
    card = add_rounded_rect(slide, x, y, Inches(2.9), Inches(2.4), CARD_BG)
    card.line.color.rgb = CARD_BORDER
    card.line.width = Pt(1)
    add_text_box(slide, x + Inches(0.2), y + Inches(0.2), Inches(2.5), Inches(0.4),
                 title, font_size=16, color=GLASS_GREEN, bold=True, alignment=PP_ALIGN.CENTER)
    add_multiline_box(slide, x + Inches(0.2), y + Inches(0.8), Inches(2.5), Inches(1.4),
                      desc.split("\n"), font_size=12, color=LIGHT_GRAY,
                      alignment=PP_ALIGN.CENTER)

add_slide_number(slide, 8, TOTAL_SLIDES)

# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
# Slide 9: Compilation Pipeline
# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
slide = prs.slides.add_slide(prs.slide_layouts[6])
add_bg(slide)
add_top_bar(slide)

add_text_box(slide, Inches(0.8), Inches(0.5), Inches(11.7), Inches(0.8),
             "ظ…ظ† ط§ظ„ظ†طµ ط¥ظ„ظ‰ ط§ظ„طھظ†ظپظٹط°", font_size=36, color=WHITE, bold=True,
             alignment=PP_ALIGN.RIGHT)

add_section_line(slide, Inches(7), Inches(1.2), Inches(5.5))

pipeline = [
    ("ط¨ط±ظ†ط§ظ…ط¬ ط¶", GLASS_GREEN),
    ("Lexer", GLASS_GREEN_DARK),
    ("Tokens", GLASS_GREEN),
    ("Parser", GLASS_GREEN_DARK),
    ("AST", GLASS_GREEN),
    ("Optimizer", GLASS_GREEN_DARK),
    ("CodeGen", GLASS_GREEN),
    ("C++20", GLASS_GREEN_DARK),
]

box_w = Inches(1.35)
box_h = Inches(0.8)
start_x = Inches(0.4)
y_pos = Inches(2.5)

for i, (text, color) in enumerate(pipeline):
    x = start_x + i * Inches(1.6)
    add_box(slide, x, y_pos, box_w, box_h, text, bg_color=color,
            text_color=WHITE, font_size=12)
    if i < len(pipeline) - 1:
        add_arrow(slide, x + box_w + Inches(0.05), y_pos + Inches(0.25),
                  Inches(0.2), Inches(0.3), color=GLASS_GREEN_DARK)

descs = [
    (0, "ط§ظ„ظƒظˆط¯ ط§ظ„ظ…طµط¯ط±ظٹ"),
    (1, "طھط­ظˆظٹظ„ UTF-8\nط¥ظ„ظ‰ Tokens"),
    (2, "ط±ظ…ظˆط² ظ…طµظ†ظپط©\n(35 ظ†ظˆط¹)"),
    (3, "ط¨ظ†ط§ط، ط´ط¬ط±ط©\nط§ظ„ظ†ط­ظˆ"),
    (4, "ط´ط¬ط±ط© ظ†ط­ظˆ\nظ…ط¬ط±ط¯ط©"),
    (5, "Constant\nFolding"),
    (6, "طھظˆظ„ظٹط¯ ظƒظˆط¯\nC++20"),
    (7, "ظƒظˆط¯ ظ†ط¸ظٹظپ\nط¬ط§ظ‡ط² ظ„ظ„طھط±ط¬ظ…ط©"),
]

for i, desc in descs:
    x = start_x + i * Inches(1.6)
    add_multiline_box(slide, x, y_pos + Inches(1.1), box_w, Inches(1.5),
                      desc.split("\n"), font_size=10, color=LIGHT_GRAY,
                      alignment=PP_ALIGN.CENTER)

add_text_box(slide, Inches(0.5), Inches(5.5), Inches(12.3), Inches(0.5),
             "ظ…ط³ط§ط± ط§ظ„ظˆظٹط¨:  ط¶ â†’ JavaScript â†’ طھظ†ظپظٹط° ظپظٹ ط§ظ„ظ…طھطµظپط­", font_size=14,
             color=MID_GRAY, alignment=PP_ALIGN.CENTER)

add_slide_number(slide, 9, TOTAL_SLIDES)

# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
# Slide 10: Lexer
# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
slide = prs.slides.add_slide(prs.slide_layouts[6])
add_bg(slide)
add_top_bar(slide)

add_text_box(slide, Inches(0.8), Inches(0.5), Inches(11.7), Inches(0.8),
             "Lexer â€” ط§ظ„ظ…ط­ظ„ظ„ ط§ظ„ظ„ظپط¸ظٹ", font_size=36, color=WHITE, bold=True,
             alignment=PP_ALIGN.RIGHT)

add_section_line(slide, Inches(7), Inches(1.2), Inches(5.5))

add_text_box(slide, Inches(0.5), Inches(1.6), Inches(6), Inches(0.5),
             "ط§ظ„ط¯ظˆط±", font_size=20, color=GLASS_GREEN, bold=True,
             alignment=PP_ALIGN.RIGHT)

lexer_items = [
    "ظٹظ‚ط±ط£ ط§ظ„ظƒظˆط¯ ط§ظ„ظ…طµط¯ط±ظٹ (UTF-8)",
    "ظٹط­ظˆظ‘ظ„ ط§ظ„ط£ط­ط±ظپ ط¥ظ„ظ‰ codepoints",
    "ظٹطµظ†ظ‘ظپ ظƒظ„ ط±ظ…ط² ط­ط³ط¨ ظ†ظˆط¹ظ‡",
    "ظٹط¯ط¹ظ… ط§ظ„ط£ط­ط±ظپ ط§ظ„ط¹ط±ط¨ظٹط© ظپظٹ ط§ظ„ظ…ط¹ط±ظپط§طھ",
    "ظٹطھط®ط·ظ‰ ط§ظ„طھط¹ظ„ظٹظ‚ط§طھ (// ظˆ /* */)",
    "ظٹظƒطھط´ظپ ط§ظ„ط£ط®ط·ط§ط، (ظ†طµ ط؛ظٹط± ظ…ط؛ظ„ظ‚...)",
]
add_bullet_list(slide, Inches(0.5), Inches(2.2), Inches(6), Inches(4),
                lexer_items, font_size=15, color=LIGHT_GRAY)

add_text_box(slide, Inches(7.5), Inches(1.6), Inches(5.3), Inches(0.5),
             "ظ…ط«ط§ظ„", font_size=20, color=GLASS_GREEN, bold=True,
             alignment=PP_ALIGN.RIGHT)

add_code_box(slide, Inches(7.5), Inches(2.2), Inches(5.3), Inches(0.8),
             'طµط­ظٹط­ ط³ = 10 ط›', font_size=15)

add_text_box(slide, Inches(7.5), Inches(3.2), Inches(5.3), Inches(0.4),
             "â†“", font_size=22, color=GLASS_GREEN, alignment=PP_ALIGN.CENTER)

token_code = """TOKEN_KEYWORD  "طµط­ظٹط­"
TOKEN_IDENTIFIER  "ط³"
TOKEN_EQUALS  "="
TOKEN_NUMBER  "10"
TOKEN_SEMICOLON  "ط›"
TOKEN_EOF"""
add_code_box(slide, Inches(7.5), Inches(3.7), Inches(5.3), Inches(2.8),
             token_code, font_size=12)

add_slide_number(slide, 10, TOTAL_SLIDES)

# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
# Slide 11: Parser
# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
slide = prs.slides.add_slide(prs.slide_layouts[6])
add_bg(slide)
add_top_bar(slide)

add_text_box(slide, Inches(0.8), Inches(0.5), Inches(11.7), Inches(0.8),
             "Parser â€” ظ…ط­ظ„ظ„ ط§ظ„ظ†ط­ظˆ", font_size=36, color=WHITE, bold=True,
             alignment=PP_ALIGN.RIGHT)

add_section_line(slide, Inches(8), Inches(1.2), Inches(4.5))

add_text_box(slide, Inches(0.5), Inches(1.6), Inches(6), Inches(0.5),
             "ط§ظ„ط¢ظ„ظٹظ‘ط©", font_size=20, color=GLASS_GREEN, bold=True,
             alignment=PP_ALIGN.RIGHT)

parser_items = [
    "Recursive Descent Parser",
    "ظٹظ‚ط±ط£ ظ…ظ† Lexer ط¹ط¨ط± consume()",
    "ظٹط¨ظ†ظٹ AST ط¹ط¨ط± ط¯ظˆط§ظ„ parse*",
    "ظٹط¯ط¹ظ… 20+ ظ†ظˆط¹ ط¬ظ…ظ„ط©",
    "ط£ظˆظ„ظˆظٹط© ط¹ظ…ظ„ظٹط§طھ: || < && < == < +/- < */% < ^",
    "1319 ط³ط·ط± ظƒظˆط¯ ظپط¹ظ„ظٹ â€” طھط­ظ‚ظ‚ 2026-08-14: ط§ظ„ظپط¹ظ„ظٹ 1378 ط³ط·ط±ظ‹ط§",
]
add_bullet_list(slide, Inches(0.5), Inches(2.2), Inches(6), Inches(4),
                parser_items, font_size=15, color=LIGHT_GRAY)

add_text_box(slide, Inches(7.5), Inches(1.6), Inches(5.3), Inches(0.5),
             "ط§ظ„طھط­ظˆظٹظ„", font_size=20, color=GLASS_GREEN, bold=True,
             alignment=PP_ALIGN.RIGHT)

add_box(slide, Inches(8.5), Inches(2.3), Inches(3.5), Inches(0.6),
        "Tokens[]", bg_color=GLASS_GREEN_DARK, font_size=13)
add_text_box(slide, Inches(8.5), Inches(3.0), Inches(3.5), Inches(0.4),
             "â†“", font_size=18, color=GLASS_GREEN, alignment=PP_ALIGN.CENTER)
add_box(slide, Inches(8.5), Inches(3.5), Inches(3.5), Inches(0.6),
        "Parser.parseStatement()", bg_color=GLASS_GREEN, font_size=13)
add_text_box(slide, Inches(8.5), Inches(4.2), Inches(3.5), Inches(0.4),
             "â†“", font_size=18, color=GLASS_GREEN, alignment=PP_ALIGN.CENTER)
add_box(slide, Inches(8.5), Inches(4.7), Inches(3.5), Inches(0.6),
        "vector<StmtAST>", bg_color=GLASS_GREEN_DARK, font_size=13)

add_slide_number(slide, 11, TOTAL_SLIDES)

# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
# Slide 12: AST
# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
slide = prs.slides.add_slide(prs.slide_layouts[6])
add_bg(slide)
add_top_bar(slide)

add_text_box(slide, Inches(0.8), Inches(0.5), Inches(11.7), Inches(0.8),
             "AST â€” ط´ط¬ط±ط© ط§ظ„ظ†ط­ظˆ ط§ظ„ظ…ط¬ط±ط¯ط©", font_size=36, color=WHITE, bold=True,
             alignment=PP_ALIGN.RIGHT)

add_section_line(slide, Inches(6.5), Inches(1.2), Inches(6))

add_text_box(slide, Inches(0.5), Inches(1.5), Inches(12.3), Inches(0.5),
             "29 ظ†ظˆط¹ ط¹ظ‚ط¯ط©  آ·  ظ†ط¸ط§ظ… Visitor Pattern  آ·  ط¯ط¹ظ… ظƒط§ظ…ظ„ ظ„ظ„ط؛ط©", font_size=16,
             color=LIGHT_GRAY, alignment=PP_ALIGN.CENTER)

add_text_box(slide, Inches(0.5), Inches(2.3), Inches(6), Inches(0.5),
             "ط¹ط¨ط§ط±ط§طھ (Expressions)", font_size=18, color=GLASS_GREEN, bold=True,
             alignment=PP_ALIGN.RIGHT)

exprs = [
    "NumberExprAST â€” ط±ظ‚ظ…",
    "VariableExprAST â€” ظ…طھط؛ظٹط±",
    "StringExprAST â€” ظ†طµ",
    "BinaryExprAST â€” ط¹ظ…ظ„ظٹط© ط«ظ†ط§ط¦ظٹط©",
    "UnaryExprAST â€” ط¹ظ…ظ„ظٹط© ط£ط­ط§ط¯ظٹط©",
    "FunctionCallAST â€” ط§ط³طھط¯ط¹ط§ط، ط¯ط§ظ„ط©",
    "MemberAccessExprAST â€” ظˆطµظˆظ„ ظ„ط¹ط¶ظˆ",
    "ArraySubscriptExprAST â€” ظپظ‡ط±ط³ ظ…طµظپظˆظپط©",
]
add_bullet_list(slide, Inches(0.5), Inches(2.9), Inches(6), Inches(3.5),
                exprs, font_size=13, color=LIGHT_GRAY)

add_text_box(slide, Inches(7), Inches(2.3), Inches(6), Inches(0.5),
             "ط¬ظ…ظ„ (Statements)", font_size=18, color=GLASS_GREEN, bold=True,
             alignment=PP_ALIGN.RIGHT)

stmts = [
    "VarDeclStmtAST â€” طھط¹ط±ظٹظپ ظ…طھط؛ظٹط±",
    "IfStmtAST â€” ط´ط±ط·",
    "WhileStmtAST â€” ط­ظ„ظ‚ط©",
    "ForStmtAST â€” ط­ظ„ظ‚ط© for",
    "FunctionDeclAST â€” طھط¹ط±ظٹظپ ط¯ط§ظ„ط©",
    "ClassDeclAST â€” طھط¹ط±ظٹظپ طµظ†ظپ",
    "TryCatchStmtAST â€” ظ…ط¹ط§ظ„ط¬ط© ط£ط®ط·ط§ط،",
    "+ 13 ظ†ظˆط¹ط§ظ‹ ط¢ط®ط±",
]
add_bullet_list(slide, Inches(7), Inches(2.9), Inches(6), Inches(3.5),
                stmts, font_size=13, color=LIGHT_GRAY)

add_slide_number(slide, 12, TOTAL_SLIDES)

# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
# Slide 13: Type System
# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
slide = prs.slides.add_slide(prs.slide_layouts[6])
add_bg(slide)
add_top_bar(slide)

add_text_box(slide, Inches(0.8), Inches(0.5), Inches(11.7), Inches(0.8),
             "ظ†ط¸ط§ظ… ط§ظ„ط£ظ†ظˆط§ط¹", font_size=36, color=WHITE, bold=True,
             alignment=PP_ALIGN.RIGHT)

add_section_line(slide, Inches(9), Inches(1.2), Inches(3.5))

types = [
    ("طµط­ظٹط­", "int", "ط£ط¹ط¯ط§ط¯ طµط­ظٹط­ط©"),
    ("ط¹ط´ط±ظٹ", "double", "ط£ط¹ط¯ط§ط¯ ط¹ط´ط±ظٹط©"),
    ("ظ…ظ†ط·ظ‚ظٹ", "bool", "طµظˆط§ط¨ / ط®ط·ط£"),
    ("ط­ط±ظپ", "char", "ط­ط±ظپ ظˆط§ط­ط¯"),
    ("ظ†طµ", "std::string", "ظ†طµظˆطµ"),
    ("ظپط±ط§ط؛", "void", "ظ„ط§ ظ‚ظٹظ…ط©"),
    ("طھظ„ظ‚ط§ط¦ظٹ", "auto", "طھظ„ظ‚ط§ط¦ظٹ"),
    ("طµط­ظٹط­[]", "std::vector<int>", "ظ…طµظپظˆظپط© ط£ط¹ط¯ط§ط¯"),
]

add_box(slide, Inches(7.5), Inches(1.6), Inches(2), Inches(0.45),
        "ظپظٹ ط¶", bg_color=GLASS_GREEN, font_size=13)
add_box(slide, Inches(5.3), Inches(1.6), Inches(2.1), Inches(0.45),
        "ظپظٹ C++", bg_color=GLASS_GREEN_DARK, font_size=13)
add_box(slide, Inches(0.5), Inches(1.6), Inches(4.7), Inches(0.45),
        "ط§ظ„ط§ط³طھط®ط¯ط§ظ…", bg_color=DARK_GRAY, font_size=13)

for i, (daad, cpp, usage) in enumerate(types):
    y = Inches(2.2 + i * 0.58)
    bg = CARD_BG if i % 2 == 0 else RGBColor(0x1E, 0x1E, 0x1E)
    add_rect(slide, Inches(0.5), y, Inches(9), Inches(0.52), bg)
    add_text_box(slide, Inches(7.5), y + Inches(0.06), Inches(2), Inches(0.4),
                 daad, font_size=13, color=GLASS_GREEN, bold=True, alignment=PP_ALIGN.CENTER)
    add_text_box(slide, Inches(5.3), y + Inches(0.06), Inches(2.1), Inches(0.4),
                 cpp, font_size=12, color=LIGHT_GRAY, alignment=PP_ALIGN.CENTER)
    add_text_box(slide, Inches(0.5), y + Inches(0.06), Inches(4.7), Inches(0.4),
                 usage, font_size=13, color=LIGHT_GRAY, alignment=PP_ALIGN.RIGHT)

add_text_box(slide, Inches(0.5), Inches(6.5), Inches(12.3), Inches(0.5),
             "ظ…ظ„ط§ط­ط¸ط©: ظ„ط§ ظٹظˆط¬ط¯ Type Checker ظ…ط³طھظ‚ظ„ â€” ط§ظ„طھط­ظ‚ظ‚ ظٹطھظ… ط¹ط¨ط± C++ ط§ظ„ظ…طھط±ط¬ظ… ط§ظ„ط®ط§ط±ط¬ظٹ",
             font_size=12, color=MID_GRAY, alignment=PP_ALIGN.CENTER)

add_slide_number(slide, 13, TOTAL_SLIDES)

# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
# Slide 14: Stdlib & Runtime
# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
slide = prs.slides.add_slide(prs.slide_layouts[6])
add_bg(slide)
add_top_bar(slide)

add_text_box(slide, Inches(0.8), Inches(0.5), Inches(11.7), Inches(0.8),
             "ط§ظ„ظ…ظƒطھط¨ط© ط§ظ„ظ‚ظٹط§ط³ظٹط© ظˆ Runtime", font_size=36, color=WHITE, bold=True,
             alignment=PP_ALIGN.RIGHT)

add_section_line(slide, Inches(5.5), Inches(1.2), Inches(7))

add_text_box(slide, Inches(0.5), Inches(1.6), Inches(6), Inches(0.5),
             "C++ Stdlib (ط³ط·ط­ ط§ظ„ظ…ظƒطھط¨)", font_size=18, color=GLASS_GREEN, bold=True,
             alignment=PP_ALIGN.RIGHT)

cpp_items = [
    "DaadStdlib.hpp â€” ط§ظ„ظ…ظƒطھط¨ط© ط§ظ„ظ…ظˆط­ط¯ط©",
    "Math.hpp â€” ط±ظٹط§ط¶ظٹط§طھ",
    "String.hpp â€” ظ†طµظˆطµ",
    "10 ظ…ط¬ظ„ط¯ط§طھ ظپط±ط¹ظٹط©",
    "~20+ ط¯ط§ظ„ط© ظ…طھظˆظپط±ط© ظپط¹ظ„ظٹط§ظ‹",
    "Runtime: daad_print ط¹ط¨ط± 3 ظ…ظ†طµط§طھ",
]
add_bullet_list(slide, Inches(0.5), Inches(2.2), Inches(6), Inches(3.5),
                cpp_items, font_size=14, color=LIGHT_GRAY)

add_text_box(slide, Inches(7), Inches(1.6), Inches(6), Inches(0.5),
             "JavaScript Stdlib (ط§ظ„ظˆظٹط¨)", font_size=18, color=GLASS_GREEN, bold=True,
             alignment=PP_ALIGN.RIGHT)

web_items = [
    "15 ظ…ظƒطھط¨ط© ظ‚ظٹط§ط³ظٹط©",
    "200+ ط¯ط§ظ„ط© ظ…طھظˆظپط±ط©",
    "ظ†طµظˆطµطŒ ط±ظٹط§ط¶ظٹط§طھطŒ ظ…طµظپظˆظپط§طھ",
    "ط±ط³ظˆظ…ظٹط§طھطŒ ط´ط¨ظƒط§طھطŒ طھط®ط²ظٹظ†",
    "ط£ط­ط¯ط§ط«طŒ DOMطŒ ط£ظ…ط§ظ†",
    "ظ…ط­ط±ظƒ ط£ظ„ط¹ط§ط¨ (35+ ط¯ط§ظ„ط©)",
]
add_bullet_list(slide, Inches(7), Inches(2.2), Inches(6), Inches(3.5),
                web_items, font_size=14, color=LIGHT_GRAY)

add_slide_number(slide, 14, TOTAL_SLIDES)

# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
# Slide 15: Current Status
# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
slide = prs.slides.add_slide(prs.slide_layouts[6])
add_bg(slide)
add_top_bar(slide)

add_text_box(slide, Inches(0.8), Inches(0.5), Inches(11.7), Inches(0.8),
             "ط£ظٹظ† ظˆطµظ„ ط§ظ„ظ…ط´ط±ظˆط¹طں", font_size=36, color=WHITE, bold=True,
             alignment=PP_ALIGN.RIGHT)

add_section_line(slide, Inches(7), Inches(1.2), Inches(5.5))

status_data = [
    ("âœ“ ظ…ظ†ظپط°", [
        "Lexer ظٹط¯ط¹ظ… Unicode ط¨ط§ظ„ظƒط§ظ…ظ„",
        "Parser ظ…ط¹ 1378 ط³ط·ط± (طھط­ظ‚ظ‚ 2026-08-14)",
        "AST ظ…ط¹ 65 ظ†ظˆط¹ ط¹ظ‚ط¯ط© (طھط­ظ‚ظ‚: AST.hpp)",
        "CodeGen â†’ C++20 + JavaScript",
        "Compiler ظƒط§ظ…ظ„",
        "SandboxValidator",
        "ط§ط®طھط¨ط§ط±ط§طھ GoogleTest 239 (12 ط³ظˆظٹطھط©طŒ طھط­ظ‚ظ‚ 2026-08-14) â€” ظƒط§ظ†طھ "~100"",
], GLASS_GREEN),
    ("â—گ ط¬ط²ط¦ظٹ", [
        "MPP ط§ظ„ظ‚ظٹط§ط³ظٹط© C++ (~20 ط¯ط§ظ„ط©)",
        "OOP (ط¨ط¯ظˆظ† ظˆط±ط§ط«ط© ظپط¹ظ„ظٹط©)",
        "Template (ط¨ط¯ظˆظ† instantiation)",
        "Qt5 IDE",
        "VS Code Extension",
    ], RGBColor(0xFF, 0xAA, 0x00)),
    ("â—¯ ظ…ط®ط·ط·", [
        "GUI widgets (14 ظƒظ„ظ…ط© ظ…ظپطھط§ط­ظٹط©)",
        "ظ…ط¹ط§ظ„ط¬ط© ط§ظ„طµظˆط± (20 ظƒظ„ظ…ط©)",
        "const/thawabet",
        "pointer/reference",
    ], RGBColor(0x55, 0xAA, 0xFF)),
    ("âœ— ط؛ظٹط± ظ…ظˆط¬ظˆط¯", [
        "Semantic Analysis",
        "IR (Intermediate Representation)",
        "Backend ظ…ظ†ط®ظپط¶ ط§ظ„ظ…ط³طھظˆظ‰",
        "Assembly / Machine Code",
    ], RGBColor(0xFF, 0x55, 0x55)),
]

x_positions = [Inches(0.3), Inches(3.4), Inches(6.5), Inches(9.6)]

for idx, (status, items, color) in enumerate(status_data):
    x = x_positions[idx]
    y = Inches(1.6)
    add_box(slide, x, y, Inches(2.9), Inches(0.45), status,
            bg_color=color, font_size=12)
    for i, item in enumerate(items):
        iy = y + Inches(0.6 + i * 0.58)
        add_text_box(slide, x + Inches(0.1), iy, Inches(2.7), Inches(0.5),
                     f"â€¢ {item}", font_size=10, color=LIGHT_GRAY,
                     alignment=PP_ALIGN.RIGHT)

add_slide_number(slide, 15, TOTAL_SLIDES)

# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
# Slide 16: Achievements
# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
slide = prs.slides.add_slide(prs.slide_layouts[6])
add_bg(slide)
add_top_bar(slide)

add_text_box(slide, Inches(0.8), Inches(0.5), Inches(11.7), Inches(0.8),
             "ظ…ط§ طھظ… ط¥ظ†ط¬ط§ط²ظ‡", font_size=36, color=WHITE, bold=True,
             alignment=PP_ALIGN.RIGHT)

add_section_line(slide, Inches(9), Inches(1.2), Inches(3.5))

achievements = [
    ("ظ…طھط±ط¬ظ… ظƒط§ظ…ظ„", "Lexer â†’ Parser â†’ AST â†’ CodeGen\nظٹظڈط®ط±ط¬ C++20 ط£ظˆ JavaScript", "01"),
    ("ط¨ظٹط¦ط© ظˆظٹط¨ طھظپط§ط¹ظ„ظٹط©", "ظ…ط­ط±ط± + ط¨ظٹط¦ط© طھظ†ظپظٹط° ظپظٹ ط§ظ„ظ…طھطµظپط­\nط¨ط¯ظˆظ† طھط«ط¨ظٹطھ", "02"),
    ("ظ†ط¸ط§ظ… ط£ظ…ط§ظ†", "SandboxValidator ظٹظپط­طµ\n22+ ظ†ظ…ط·ط§ظ‹ ط®ط·ظٹط±ط§ظ‹", "03"),
    ("ظ…ظ†طµط© طھط¹ظ„ظٹظ…ظٹط©", "Backend API + ط¯ظˆط±ط§طھ\nظˆطھط­ط¯ظٹط§طھ طھظپط§ط¹ظ„ظٹط©", "04"),
]

for i, (title, desc, num) in enumerate(achievements):
    col = i % 2
    row = i // 2
    x = Inches(0.5 + col * 6.5)
    y = Inches(1.8 + row * 2.6)
    card = add_rounded_rect(slide, x, y, Inches(5.8), Inches(2.1), CARD_BG)
    card.line.color.rgb = CARD_BORDER
    card.line.width = Pt(1)
    add_text_box(slide, x + Inches(4.5), y + Inches(0.2), Inches(1), Inches(0.7),
                 num, font_size=36, color=GLASS_GREEN, bold=True,
                 alignment=PP_ALIGN.CENTER, font_name="Segoe UI Light")
    add_text_box(slide, x + Inches(0.3), y + Inches(0.2), Inches(4), Inches(0.5),
                 title, font_size=20, color=WHITE, bold=True, alignment=PP_ALIGN.RIGHT)
    add_multiline_box(slide, x + Inches(0.3), y + Inches(0.9), Inches(4), Inches(1),
                      desc.split("\n"), font_size=13, color=LIGHT_GRAY,
                      alignment=PP_ALIGN.RIGHT)

add_slide_number(slide, 16, TOTAL_SLIDES)

# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
# Slide 17: Incomplete
# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
slide = prs.slides.add_slide(prs.slide_layouts[6])
add_bg(slide)
add_top_bar(slide)

add_text_box(slide, Inches(0.8), Inches(0.5), Inches(11.7), Inches(0.8),
             "ظ…ط§ ظ„ظ… ظٹظƒطھظ…ظ„ ط¨ط¹ط¯", font_size=36, color=WHITE, bold=True,
             alignment=PP_ALIGN.RIGHT)

add_section_line(slide, Inches(7.5), Inches(1.2), Inches(5))

incomplete = [
    ("Semantic Analysis", "ظ„ط§ ظٹظˆط¬ط¯ ظپط­طµ ط£ظ†ظˆط§ط¹ ط£ظˆ Scope ط£ظˆ Symbol Table", RGBColor(0xFF, 0x55, 0x55)),
    ("IR", "ظ„ط§ طھظˆط¬ط¯ ط·ط¨ظ‚ط© ظˆط³ظٹط·ط© ط¨ظٹظ† AST ظˆ C++", RGBColor(0xFF, 0xAA, 0x00)),
    ("Backend", "ظ„ط§ ظٹظˆط¬ط¯ backend ط®ط§طµ ط¨ظ„ط؛ط© ط¶", RGBColor(0xFF, 0x55, 0x55)),
    ("ط§ظ„ظ…ظƒطھط¨ط© ط§ظ„ظ‚ظٹط§ط³ظٹط© C++", "~20+ ط¯ط§ظ„ط© ظپظ‚ط· (ظ…ظ‚ط§ط±ظ†ط© ط¨ظ€ 200+ ظپظٹ ط§ظ„ظˆظٹط¨)", RGBColor(0x55, 0xAA, 0xFF)),
    ("OOP ط§ظ„ظƒط§ظ…ظ„", "ط¨ط¯ظˆظ† ظˆط±ط§ط«ط© ظپط¹ظ„ظٹط© ط£ظˆ polymorphism", RGBColor(0xFF, 0xAA, 0x00)),
    ("ط§ظ„ط¥ط¯ط®ط§ظ„", "ظ„ط§ طھظˆط¬ط¯ ط¯ط§ظ„ط© ظ‚ط±ط§ط،ط© ظ…ظ† ط§ظ„ظ…ط³طھط®ط¯ظ…", RGBColor(0xFF, 0x55, 0x55)),
]

for i, (title, desc, color) in enumerate(incomplete):
    y = Inches(1.7 + i * 0.85)
    card = add_rounded_rect(slide, Inches(0.5), y, Inches(12.3), Inches(0.75), CARD_BG)
    card.line.color.rgb = CARD_BORDER
    card.line.width = Pt(1)
    add_rect(slide, Inches(0.5), y, Inches(0.06), Inches(0.75), color)
    add_text_box(slide, Inches(0.85), y + Inches(0.08), Inches(3.5), Inches(0.3),
                 title, font_size=15, color=color, bold=True, alignment=PP_ALIGN.RIGHT)
    add_text_box(slide, Inches(4.5), y + Inches(0.08), Inches(8), Inches(0.3),
                 desc, font_size=13, color=LIGHT_GRAY, alignment=PP_ALIGN.RIGHT)

add_slide_number(slide, 17, TOTAL_SLIDES)

# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
# Slide 18: Daad Now
# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
slide = prs.slides.add_slide(prs.slide_layouts[6])
add_bg(slide)
add_top_bar(slide)

add_text_box(slide, Inches(0.8), Inches(0.5), Inches(11.7), Inches(0.8),
             "ظ„ط؛ط© آ«ط¶آ» ط§ظ„ط¢ظ†", font_size=36, color=WHITE, bold=True,
             alignment=PP_ALIGN.RIGHT)

add_section_line(slide, Inches(8.5), Inches(1.2), Inches(4))

add_text_box(slide, Inches(1), Inches(1.7), Inches(11.3), Inches(0.8),
             "ظ„ط؛ط© آ«ط¶آ» ط§ظ„ط­ط§ظ„ظٹط© ظ‡ظٹ ظ„ط؛ط© ط¹ط§ظ„ظٹط© ط§ظ„ظ…ط³طھظˆظ‰ طھط¹ظ…ظ„ ظƒظ€ Transpiler",
             font_size=20, color=GLASS_GREEN, bold=True, alignment=PP_ALIGN.CENTER)

add_box(slide, Inches(3.5), Inches(3.0), Inches(2.5), Inches(0.65),
        "ط¶ ط¹ط§ظ„ظٹط© ط§ظ„ظ…ط³طھظˆظ‰", bg_color=GLASS_GREEN, font_size=14)
add_text_box(slide, Inches(3.5), Inches(3.75), Inches(2.5), Inches(0.4),
             "â†“", font_size=18, color=GLASS_GREEN, alignment=PP_ALIGN.CENTER)
add_box(slide, Inches(3.5), Inches(4.1), Inches(2.5), Inches(0.65),
        "C++20 / JavaScript", bg_color=GLASS_GREEN_DARK, font_size=14)
add_text_box(slide, Inches(3.5), Inches(4.85), Inches(2.5), Inches(0.4),
             "â†“", font_size=18, color=GLASS_GREEN, alignment=PP_ALIGN.CENTER)
add_box(slide, Inches(3.5), Inches(5.2), Inches(2.5), Inches(0.65),
        "ظ…طھط±ط¬ظ… C++ ط®ط§ط±ط¬ظٹ", bg_color=DARK_GRAY, font_size=14)
add_text_box(slide, Inches(3.5), Inches(5.95), Inches(2.5), Inches(0.4),
             "â†“", font_size=18, color=GLASS_GREEN, alignment=PP_ALIGN.CENTER)
add_box(slide, Inches(3.5), Inches(6.3), Inches(2.5), Inches(0.45),
        "Machine Code", bg_color=DARK_GRAY, font_size=12)

add_text_box(slide, Inches(7), Inches(3.5), Inches(5.5), Inches(3),
             "ظ„ط§ ظٹظˆط¬ط¯ ظپظٹ ظ‡ط°ط§ ط§ظ„ظ…ط´ط±ظˆط¹:\n\nâ€¢ ط£ظٹ ظƒظˆط¯ Assembly\nâ€¢ ط£ظٹ Machine Code\nâ€¢ ط£ظٹ Backend ظ…ظ†ط®ظپط¶ ط§ظ„ظ…ط³طھظˆظ‰\nâ€¢ ط£ظٹ register allocation\nâ€¢ ط£ظٹ instruction selection\n\nط§ظ„ط§ط¹طھظ…ط§ط¯ ط§ظ„ظƒط§ظ…ظ„ ط¹ظ„ظ‰ ظ…طھط±ط¬ظ… C++ ط®ط§ط±ط¬ظٹ",
             font_size=15, color=LIGHT_GRAY, alignment=PP_ALIGN.RIGHT)

add_slide_number(slide, 18, TOTAL_SLIDES)

# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
# Slide 19: Gap Analysis
# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
slide = prs.slides.add_slide(prs.slide_layouts[6])
add_bg(slide)
add_top_bar(slide)

add_text_box(slide, Inches(0.8), Inches(0.5), Inches(11.7), Inches(0.8),
             "ظ…ط§ ط§ظ„ط°ظٹ ظ„ظ… ظٹطھظ… ط¨ط¹ط¯طں", font_size=36, color=WHITE, bold=True,
             alignment=PP_ALIGN.RIGHT)

add_section_line(slide, Inches(7), Inches(1.2), Inches(5.5))

add_text_box(slide, Inches(0.5), Inches(1.6), Inches(12.3), Inches(0.5),
             "ط§ظ„ظپط¬ظˆط© ط¨ظٹظ† ط§ظ„ظ…ط³طھظˆظ‰ ط§ظ„ط¹ط§ظ„ظٹ ظˆط§ظ„ظ…ظ†ط®ظپط¶", font_size=20, color=GLASS_GREEN,
             bold=True, alignment=PP_ALIGN.CENTER)

add_box(slide, Inches(0.5), Inches(2.5), Inches(5), Inches(1),
        "ط§ظ„ظ…ط³طھظˆظ‰ ط§ظ„ط¹ط§ظ„ظٹ\nLexer + Parser + AST + CodeGen\n(ظ…ظˆط¬ظˆط¯ ظپط¹ظ„ظٹط§ظ‹)",
        bg_color=GLASS_GREEN, font_size=12)

add_rect(slide, Inches(5.8), Inches(2.5), Inches(1.7), Inches(1), RGBColor(0xFF, 0x55, 0x55))
add_text_box(slide, Inches(5.8), Inches(2.7), Inches(1.7), Inches(0.6),
             "ظپط¬ظˆط©\n طھظ‚ظ†ظٹط©", font_size=14, color=WHITE, bold=True,
             alignment=PP_ALIGN.CENTER)

add_box(slide, Inches(7.8), Inches(2.5), Inches(5), Inches(1),
        "ط§ظ„ظ…ط³طھظˆظ‰ ط§ظ„ظ…ظ†ط®ظپط¶\nAssembly + Machine Code\n(ط؛ظٹط± ظ…ظˆط¬ظˆط¯)",
        bg_color=RGBColor(0xFF, 0x55, 0x55), font_size=12)

add_text_box(slide, Inches(0.5), Inches(4.0), Inches(12.3), Inches(0.5),
             "ط§ظ„ظ…ظƒظˆظ†ط§طھ ط§ظ„ظ…ط·ظ„ظˆط¨ط© ظ„ط³ط¯ ط§ظ„ظپط¬ظˆط©:", font_size=16, color=GLASS_GREEN,
             bold=True, alignment=PP_ALIGN.RIGHT)

needed = [
    "Semantic Analysis â€” ظپط­طµ ط§ظ„ط£ظ†ظˆط§ط¹ ظˆط§ظ„ظ€ Scope",
    "IR â€” Intermediate Representation â€” طھظ…ط«ظٹظ„ ظˆط³ظٹط·",
    "Backend â€” طھظˆظ„ظٹط¯ ظƒظˆط¯ ظ…ظ†ط®ظپط¶ ط§ظ„ظ…ط³طھظˆظ‰",
    "Register Allocation â€” طھظˆط²ظٹط¹ ط§ظ„ط³ط¬ظ„ط§طھ",
    "Assembler â€” طھط­ظˆظٹظ„ Assembly ط¥ظ„ظ‰ binary",
]
add_bullet_list(slide, Inches(0.5), Inches(4.6), Inches(12.3), Inches(2.5),
                needed, font_size=14, color=LIGHT_GRAY)

add_slide_number(slide, 19, TOTAL_SLIDES)

# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
# Slide 20: Future Vision
# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
slide = prs.slides.add_slide(prs.slide_layouts[6])
add_bg(slide)
add_top_bar(slide)

add_text_box(slide, Inches(0.8), Inches(0.5), Inches(11.7), Inches(0.8),
             "ظ…ظ† آ«ط¶آ» ط¹ط§ظ„ظٹط© ط§ظ„ظ…ط³طھظˆظ‰ ط¥ظ„ظ‰ طھط¹ظ„ظٹظ…ط§طھ ط§ظ„ظ…ط¹ط§ظ„ط¬", font_size=34,
             color=WHITE, bold=True, alignment=PP_ALIGN.RIGHT)

add_section_line(slide, Inches(4), Inches(1.2), Inches(8.5))

add_text_box(slide, Inches(0.5), Inches(1.5), Inches(12.3), Inches(0.4),
             "ط§ظ„ظ…ط¹ظ…ط§ط±ظٹط© ط§ظ„ظ…ط³طھظ‚ط¨ظ„ظٹط© ط§ظ„ظ…ظ‚طھط±ط­ط©", font_size=14, color=MID_GRAY,
             alignment=PP_ALIGN.CENTER)

future_steps = [
    ("ط¶ ط¹ط§ظ„ظٹط© ط§ظ„ظ…ط³طھظˆظ‰", GLASS_GREEN, True),
    ("Lexer", GLASS_GREEN, True),
    ("Parser", GLASS_GREEN, True),
    ("AST", GLASS_GREEN, True),
    ("Semantic Analysis", RGBColor(0x55, 0xAA, 0xFF), False),
    ("IR", RGBColor(0x55, 0xAA, 0xFF), False),
    ("Backend", RGBColor(0xFF, 0xAA, 0x00), False),
    ("Assembly", RGBColor(0xFF, 0xAA, 0x00), False),
    ("Machine Code", RGBColor(0xFF, 0x55, 0x55), False),
    ("CPU", RGBColor(0xFF, 0x55, 0x55), False),
]

box_w = Inches(1.1)
box_h = Inches(0.55)
start_x = Inches(0.3)
y_main = Inches(2.3)

for i, (text, color, is_done) in enumerate(future_steps):
    x = start_x + i * Inches(1.28)
    shape = add_box(slide, x, y_main, box_w, box_h, text,
                    bg_color=color if is_done else CARD_BG,
                    text_color=WHITE, font_size=9, bold=True)
    if not is_done:
        shape.line.color.rgb = color
        shape.line.width = Pt(1.5)
    if i < len(future_steps) - 1:
        add_arrow(slide, x + box_w, y_main + Inches(0.13), Inches(0.15), Inches(0.25),
                  color=GLASS_GREEN_DARK)

add_shape_bg = add_rect

add_rect(slide, Inches(0.5), Inches(3.8), Inches(12.3), Inches(2.8), CARD_BG)
add_text_box(slide, Inches(0.8), Inches(3.9), Inches(11.7), Inches(2.5),
             "ظ…ظ„ط§ط­ط¸ط©: ظ‡ط°ظ‡ ط§ظ„ظ…ط¹ظ…ط§ط±ظٹط© ظ…ظ‚طھط±ط­ط© ظˆظ„ظٹط³طھ ظ…ظ†ظپط°ط©.\n\n"
             "ط§ظ„ظ…ط±ط­ظ„ط© ط§ظ„ط­ط§ظ„ظٹط© طھط±ظƒط² ط¹ظ„ظ‰ طھط­ط³ظٹظ† ط§ظ„ظ…ط³طھظˆظ‰ ط§ظ„ط¹ط§ظ„ظٹ (Lexer + Parser + AST + CodeGen).\n"
             "ط§ظ„ظ…ط±ط­ظ„ط© ط§ظ„ظ…ط³طھظ‚ط¨ظ„ظٹط© طھطھط·ظ„ط¨ ط¨ظ†ط§ط، Semantic Analysis ط£ظˆظ„ط§ظ‹طŒ ط«ظ… IRطŒ ط«ظ… Backend.\n\n"
             "ط§ظ„ط®ظٹط§ط± ط§ظ„ط£ظˆظ„ ظ‡ظˆ ط§ط³طھط®ط¯ط§ظ… LLVM ظ„ط¯ط¹ظ… ظ…ط¹ط§ظ„ط¬ط§طھ ظ…طھط¹ط¯ط¯ط© (x86, ARM, RISC-V).",
             font_size=13, color=LIGHT_GRAY, alignment=PP_ALIGN.RIGHT)

add_slide_number(slide, 20, TOTAL_SLIDES)

# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
# Slide 21: Why IR?
# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
slide = prs.slides.add_slide(prs.slide_layouts[6])
add_bg(slide)
add_top_bar(slide)

add_text_box(slide, Inches(0.8), Inches(0.5), Inches(11.7), Inches(0.8),
             "ظ„ظ…ط§ط°ط§ ظ†ط­طھط§ط¬ ط·ط¨ظ‚ط© ظˆط³ظٹط·ط© (IR)طں", font_size=36, color=WHITE, bold=True,
             alignment=PP_ALIGN.RIGHT)

add_section_line(slide, Inches(5.5), Inches(1.2), Inches(7))

benefits = [
    ("ظپطµظ„ ط§ظ„ظ„ط؛ط© ط¹ظ† ط§ظ„ظ…ط¹ط§ظ„ط¬", "ظٹط³ظ…ط­ ط¨ط¨ظ†ط§ط، ط£ظƒط«ط± ظ…ظ† Backend\nط¨ط¯ظˆظ† طھط؛ظٹظٹط± Frontend"),
    ("طھط­ط³ظٹظ† ط§ظ„ط¨ط±ظ†ط§ظ…ط¬", "ظٹظ…ظƒظ† طھط·ط¨ظٹظ‚ طھط­ط³ظٹظ†ط§طھ\nط¹ظ„ظ‰ ظ…ط³طھظˆظ‰ IR ظ…ط¨ط§ط´ط±ط©"),
    ("ط¯ط¹ظ… ظ…ط¹ظ…ط§ط±ظٹط§طھ ظ…طھط¹ط¯ط¯ط©", "x86, ARM, RISC-V\nظ…ظ† IR ظˆط§ط­ط¯"),
    ("طھط³ظ‡ظٹظ„ ط§ظ„ط§ط®طھط¨ط§ط±", "ظٹظ…ظƒظ† ط§ط®طھط¨ط§ط± IR ط¨ط´ظƒظ„\nظ…ط³طھظ‚ظ„ ط¹ظ† Backend"),
]

for i, (title, desc) in enumerate(benefits):
    col = i % 2
    row = i // 2
    x = Inches(0.5 + col * 6.5)
    y = Inches(1.8 + row * 2.6)
    card = add_rounded_rect(slide, x, y, Inches(5.8), Inches(2.1), CARD_BG)
    card.line.color.rgb = CARD_BORDER
    card.line.width = Pt(1)
    add_text_box(slide, x + Inches(0.3), y + Inches(0.3), Inches(5.2), Inches(0.5),
                 title, font_size=20, color=GLASS_GREEN, bold=True, alignment=PP_ALIGN.RIGHT)
    add_multiline_box(slide, x + Inches(0.3), y + Inches(1.0), Inches(5.2), Inches(1),
                      desc.split("\n"), font_size=14, color=LIGHT_GRAY,
                      alignment=PP_ALIGN.RIGHT)

add_slide_number(slide, 21, TOTAL_SLIDES)

# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
# Slide 22: Transformation Concept
# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
slide = prs.slides.add_slide(prs.slide_layouts[6])
add_bg(slide)
add_top_bar(slide)

add_text_box(slide, Inches(0.8), Inches(0.5), Inches(11.7), Inches(0.8),
             "ظƒظٹظپ ظٹظ…ظƒظ† ط£ظ† ظٹط­ط¯ط« ط§ظ„طھط­ظˆظٹظ„طں", font_size=36, color=WHITE, bold=True,
             alignment=PP_ALIGN.RIGHT)

add_section_line(slide, Inches(5), Inches(1.2), Inches(7.5))

add_text_box(slide, Inches(0.5), Inches(1.5), Inches(12.3), Inches(0.4),
             "ظ…ط«ط§ظ„ طھطµظˆط±ظٹ â€” ظ„ظٹط³ syntax ظ†ظ‡ط§ط¦ظٹ", font_size=12, color=MID_GRAY,
             alignment=PP_ALIGN.CENTER)

add_text_box(slide, Inches(0.5), Inches(2.1), Inches(6), Inches(0.5),
             "ظƒظˆط¯ ط¶:", font_size=16, color=GLASS_GREEN, bold=True,
             alignment=PP_ALIGN.RIGHT)

add_code_box(slide, Inches(0.5), Inches(2.7), Inches(6), Inches(1.5),
             "طµط­ظٹط­ ط³ = 10 + 5 ط›\nط·ط¨ط§ط¹ط©( ط³ ) ط›", font_size=15)

add_text_box(slide, Inches(7), Inches(2.1), Inches(6), Inches(0.5),
             "IR ط§ظ„ظ…ظƒطھظˆط¨ (طھطµظˆط±ظٹ):", font_size=16, color=GLASS_GREEN, bold=True,
             alignment=PP_ALIGN.RIGHT)

add_code_box(slide, Inches(7), Inches(2.7), Inches(5.8), Inches(1.5),
             "t1 = i32 10\nt2 = i32 5\nt3 = add i32 t1, t2\nstore i32 t3, @ط³\ncall @daad_print(@ط³)",
             font_size=13)

add_text_box(slide, Inches(0.5), Inches(4.5), Inches(12.3), Inches(0.5),
             "ظ…ط«ط§ظ„ طھطµظˆط±ظٹ â€” ظٹظˆط¶ط­ ط§ظ„ظ…ظپظ‡ظˆظ… ظپظ‚ط· ظˆظ„ظٹط³ ط§ظ„طھظ†ظپظٹط° ط§ظ„ط­ط§ظ„ظٹ", font_size=12,
             color=MID_GRAY, alignment=PP_ALIGN.CENTER)

add_slide_number(slide, 22, TOTAL_SLIDES)

# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
# Slide 23: Processors
# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
slide = prs.slides.add_slide(prs.slide_layouts[6])
add_bg(slide)
add_top_bar(slide)

add_text_box(slide, Inches(0.8), Inches(0.5), Inches(11.7), Inches(0.8),
             "ط§ظ„ظ…ط¹ط§ظ„ط¬ط§طھ ظ…ط³طھظ‚ط¨ظ„ط§ظ‹", font_size=36, color=WHITE, bold=True,
             alignment=PP_ALIGN.RIGHT)

add_section_line(slide, Inches(7), Inches(1.2), Inches(5.5))

add_text_box(slide, Inches(0.5), Inches(1.6), Inches(12.3), Inches(0.5),
             "ط§ظ„ظˆط¶ط¹ ط§ظ„ط­ط§ظ„ظٹ: ظ„ط§ ظٹظˆط¬ط¯ ط¯ط¹ظ… ظ„ط£ظٹ ظ…ط¹ط§ظ„ط¬", font_size=18, color=RGBColor(0xFF, 0x55, 0x55),
             bold=True, alignment=PP_ALIGN.CENTER)

add_text_box(slide, Inches(0.5), Inches(2.4), Inches(12.3), Inches(0.5),
             "ط§ظ„ظ…ط¹ط§ظ„ط¬ط§طھ ط§ظ„ظ…ظ…ظƒظ† ط¯ط¹ظ…ظ‡ط§ ظ…ط³طھظ‚ط¨ظ„ط§ظ‹:", font_size=18, color=GLASS_GREEN,
             bold=True, alignment=PP_ALIGN.CENTER)

processors = [
    ("x86-64", "ط§ظ„ظ…ط¹ط§ظ„ط¬ ط§ظ„ط£ظƒط«ط± ط´ظٹظˆط¹ط§ظ‹ ظپظٹ ط£ط¬ظ‡ط²ط© ط§ظ„ظƒظ…ط¨ظٹظˆطھط±"),
    ("ARM", "ظ…ط¹ط§ظ„ط¬ط§طھ ط§ظ„ط£ط¬ظ‡ط²ط© ط§ظ„ظ…ط­ظ…ظˆظ„ط© ظˆط§ظ„ظ„ظˆط­ط§طھ"),
    ("RISC-V", "ظ…ط¹ط§ظ„ط¬ ظ…ظپطھظˆط­ ط§ظ„ظ…طµط¯ط± ظˆط§ظ„ظ…طھظ†ط§ظ…ظٹ"),
]

for i, (name, desc) in enumerate(processors):
    x = Inches(0.5 + i * 4.2)
    y = Inches(3.2)
    card = add_rounded_rect(slide, x, y, Inches(3.8), Inches(2), CARD_BG)
    card.line.color.rgb = CARD_BORDER
    card.line.width = Pt(1)
    add_text_box(slide, x + Inches(0.2), y + Inches(0.3), Inches(3.4), Inches(0.5),
                 name, font_size=22, color=GLASS_GREEN, bold=True, alignment=PP_ALIGN.CENTER)
    add_text_box(slide, x + Inches(0.2), y + Inches(1.0), Inches(3.4), Inches(0.7),
                 desc, font_size=13, color=LIGHT_GRAY, alignment=PP_ALIGN.CENTER)

add_text_box(slide, Inches(0.5), Inches(5.8), Inches(12.3), Inches(0.5),
             "ط§ظ„ط®ظٹط§ط± ط§ظ„ط£ظپط¶ظ„ ظ‡ظˆ ط§ط³طھط®ط¯ط§ظ… LLVM ظ„ط¯ط¹ظ… ط¬ظ…ظٹط¹ ظ‡ط°ظ‡ ط§ظ„ظ…ط¹ط§ظ„ط¬ط§طھ طھظ„ظ‚ط§ط¦ظٹط§ظ‹",
             font_size=14, color=MID_GRAY, alignment=PP_ALIGN.CENTER)

add_slide_number(slide, 23, TOTAL_SLIDES)

# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
# Slide 24: Roadmap
# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
slide = prs.slides.add_slide(prs.slide_layouts[6])
add_bg(slide)
add_top_bar(slide)

add_text_box(slide, Inches(0.8), Inches(0.5), Inches(11.7), Inches(0.8),
             "ط®ط§ط±ط·ط© طھط·ظˆظٹط± آ«ط¶آ»", font_size=36, color=WHITE, bold=True,
             alignment=PP_ALIGN.RIGHT)

add_section_line(slide, Inches(7.5), Inches(1.2), Inches(5))

phases = [
    ("1", "ط§ظ„ظ…ط±ط­ظ„ط© ط§ظ„ط£ظˆظ„ظ‰", "ظ„ط؛ط© ط¹ط§ظ„ظٹط© ط§ظ„ظ…ط³طھظˆظ‰", "ظ…ظƒطھظ…ظ„ط©", GLASS_GREEN),
    ("2", "ط§ظ„ظ…ط±ط­ظ„ط© ط§ظ„ط«ط§ظ†ظٹط©", "طھط­ط³ظٹظ† ط§ظ„ظ…طھط±ط¬ظ…", "ط¬ط§ط±ظٹط©", RGBColor(0xFF, 0xAA, 0x00)),
    ("3", "ط§ظ„ظ…ط±ط­ظ„ط© ط§ظ„ط«ط§ظ„ط«ط©", "IR / Intermediate Layer", "ظ…ط³طھظ‚ط¨ظ„ظٹط©", RGBColor(0x55, 0xAA, 0xFF)),
    ("4", "ط§ظ„ظ…ط±ط­ظ„ط© ط§ظ„ط±ط§ط¨ط¹ط©", "Backend ظ…ظ†ط®ظپط¶ ط§ظ„ظ…ط³طھظˆظ‰", "ظ…ط³طھظ‚ط¨ظ„ظٹط©", RGBColor(0x55, 0xAA, 0xFF)),
    ("5", "ط§ظ„ظ…ط±ط­ظ„ط© ط§ظ„ط®ط§ظ…ط³ط©", "Assembly / Machine Code", "ظ…ط³طھظ‚ط¨ظ„ظٹط©", RGBColor(0x55, 0xAA, 0xFF)),
    ("6", "ط§ظ„ظ…ط±ط­ظ„ط© ط§ظ„ط³ط§ط¯ط³ط©", "ط¯ط¹ظ… ظ…ط¹ظ…ط§ط±ظٹط§طھ ظ…طھط¹ط¯ط¯ط©", "ظ…ط³طھظ‚ط¨ظ„ظٹط©", RGBColor(0x55, 0xAA, 0xFF)),
]

for i, (num, phase, desc, status, color) in enumerate(phases):
    y = Inches(1.6 + i * 0.9)
    circle = add_circle(slide, Inches(10.5), y, Inches(0.6), color)
    add_text_box(slide, Inches(10.5), y + Inches(0.08), Inches(0.6), Inches(0.45),
                 num, font_size=18, color=WHITE, bold=True, alignment=PP_ALIGN.CENTER)
    add_text_box(slide, Inches(5), y + Inches(0.05), Inches(5.2), Inches(0.3),
                 phase, font_size=15, color=WHITE, bold=True, alignment=PP_ALIGN.RIGHT)
    add_text_box(slide, Inches(5), y + Inches(0.35), Inches(5.2), Inches(0.3),
                 desc, font_size=13, color=LIGHT_GRAY, alignment=PP_ALIGN.RIGHT)
    add_text_box(slide, Inches(0.5), y + Inches(0.1), Inches(4), Inches(0.4),
                 status, font_size=13, color=color, bold=True, alignment=PP_ALIGN.RIGHT)
    if i < len(phases) - 1:
        add_rect(slide, Inches(10.77), y + Inches(0.6), Inches(0.04), Inches(0.3), MID_GRAY)

add_slide_number(slide, 24, TOTAL_SLIDES)

# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
# Slide 25: Strengths
# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
slide = prs.slides.add_slide(prs.slide_layouts[6])
add_bg(slide)
add_top_bar(slide)

add_text_box(slide, Inches(0.8), Inches(0.5), Inches(11.7), Inches(0.8),
             "ظ†ظ‚ط§ط· ط§ظ„ظ‚ظˆط©", font_size=36, color=WHITE, bold=True,
             alignment=PP_ALIGN.RIGHT)

add_section_line(slide, Inches(9.5), Inches(1.2), Inches(3))

strengths = [
    ("Unicode ظƒط§ظ…ظ„", "ط§ظ„ط£ط­ط±ظپ ط§ظ„ط¹ط±ط¨ظٹط© طھط¹ظ…ظ„\nظپظٹ ظƒظ„ ظ…ظƒط§ظ†"),
    ("ظ…ط¹ظ…ط§ط±ظٹط© ظˆط§ط¶ط­ط©", "ظپطµظ„ ط¬ظٹط¯ ط¨ظٹظ† ط§ظ„ظ…ظƒظˆظ†ط§طھ\nط¹ط¨ط± Visitor Pattern"),
    ("ط§ط®طھط¨ط§ط±ط§طھ", "239 ط§ط®طھط¨ط§ط± GoogleTest (12 ط³ظˆظٹطھط©)\n+ 334 ط§ط®طھط¨ط§ط± Jest (312 ظ†ط§ط¬ط­) â€” طھط­ظ‚ظ‚ 2026-08-14"),
    ("ط£ظ…ط§ظ†", "SandboxValidator\nظٹظپط­طµ ط§ظ„ظƒظˆط¯ ط§ظ„ظ…ظˆظ„ط¯"),
    ("طھظˆط§ظپظ‚ Web", "ظ…ط­ظˆظ‘ظ„ ظٹط¹ظ…ظ„ ظپظٹ ط§ظ„ظ…طھطµظپط­\nط¨ط¯ظˆظ† طھط«ط¨ظٹطھ"),
    ("ط³ظ‡ظˆظ„ط© ط§ظ„ط§ط³طھط®ط¯ط§ظ…", "CLI ط¨ط³ظٹط· + ظ…ط­ط±ط± ظˆظٹط¨\n+ ط£ظ…ط«ظ„ط© ط¬ط§ظ‡ط²ط©"),
]

for i, (title, desc) in enumerate(strengths):
    col = i % 3
    row = i // 3
    x = Inches(0.5 + col * 4.2)
    y = Inches(1.7 + row * 2.7)
    card = add_rounded_rect(slide, x, y, Inches(3.8), Inches(2.2), CARD_BG)
    card.line.color.rgb = CARD_BORDER
    card.line.width = Pt(1)
    add_text_box(slide, x + Inches(0.2), y + Inches(0.3), Inches(3.4), Inches(0.5),
                 title, font_size=18, color=GLASS_GREEN, bold=True, alignment=PP_ALIGN.CENTER)
    add_multiline_box(slide, x + Inches(0.2), y + Inches(1.0), Inches(3.4), Inches(1),
                      desc.split("\n"), font_size=13, color=LIGHT_GRAY,
                      alignment=PP_ALIGN.CENTER)

add_slide_number(slide, 25, TOTAL_SLIDES)

# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
# Slide 26: Challenges
# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
slide = prs.slides.add_slide(prs.slide_layouts[6])
add_bg(slide)
add_top_bar(slide)

add_text_box(slide, Inches(0.8), Inches(0.5), Inches(11.7), Inches(0.8),
             "ط§ظ„طھط­ط¯ظٹط§طھ", font_size=36, color=WHITE, bold=True,
             alignment=PP_ALIGN.RIGHT)

add_section_line(slide, Inches(10), Inches(1.2), Inches(2.5))

challenges = [
    ("ظ„ط§ ظٹظˆط¬ط¯ Semantic Analysis", "ظٹظ…ظ†ط¹ ط¨ظ†ط§ط، IR ظˆ Backend ط¨ط´ظƒظ„ طµط­ظٹط­", RGBColor(0xFF, 0x55, 0x55)),
    ("ظ„ط§ ظٹظˆط¬ط¯ IR", "ظٹظ…ظ†ط¹ ط§ظ„طھط­ط³ظٹظ†ط§طھ ط§ظ„ظ…طھظ‚ط¯ظ…ط© ظˆط§ظ„طھط±ط¬ظ…ط© ط§ظ„ظ…ط¨ط§ط´ط±ط©", RGBColor(0xFF, 0xAA, 0x00)),
    ("ظ„ط§ ظٹظˆط¬ط¯ Backend", "ط§ظ„ط§ط¹طھظ…ط§ط¯ ط¹ظ„ظ‰ ظ…طھط±ط¬ظ… C++ ط®ط§ط±ط¬ظٹ", RGBColor(0xFF, 0x55, 0x55)),
    ("ط§ظ„ظ…ظƒطھط¨ط© ط§ظ„ظ‚ظٹط§ط³ظٹط© ظ…ط­ط¯ظˆط¯ط©", "~20+ ط¯ط§ظ„ط© ظپظ‚ط· ظپظٹ C++", RGBColor(0x55, 0xAA, 0xFF)),
    ("OOP ظ…ط­ط¯ظˆط¯", "ط¨ط¯ظˆظ† ظˆط±ط§ط«ط© ظپط¹ظ„ظٹط© ط£ظˆ polymorphism", RGBColor(0xFF, 0xAA, 0x00)),
    ("ظ„ط§ ظٹظˆط¬ط¯ REPL", "ظ„ط§ طھظˆط¬ط¯ ط¨ظٹط¦ط© طھظپط§ط¹ظ„ظٹط© ظپظٹ ط³ط·ط­ ط§ظ„ظ…ظƒطھط¨", RGBColor(0xFF, 0x55, 0x55)),
]

for i, (title, desc, color) in enumerate(challenges):
    y = Inches(1.6 + i * 0.85)
    card = add_rounded_rect(slide, Inches(0.5), y, Inches(12.3), Inches(0.75), CARD_BG)
    card.line.color.rgb = CARD_BORDER
    card.line.width = Pt(1)
    add_rect(slide, Inches(0.5), y, Inches(0.06), Inches(0.75), color)
    add_text_box(slide, Inches(0.85), y + Inches(0.08), Inches(5), Inches(0.3),
                 title, font_size=15, color=color, bold=True, alignment=PP_ALIGN.RIGHT)
    add_text_box(slide, Inches(6), y + Inches(0.08), Inches(6.5), Inches(0.3),
                 desc, font_size=13, color=LIGHT_GRAY, alignment=PP_ALIGN.RIGHT)

add_slide_number(slide, 26, TOTAL_SLIDES)

# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
# Slide 27: Future Vision
# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
slide = prs.slides.add_slide(prs.slide_layouts[6])
add_bg(slide)
add_top_bar(slide)

add_text_box(slide, Inches(0.8), Inches(0.5), Inches(11.7), Inches(0.8),
             "ط£ظٹظ† ظٹظ…ظƒظ† ط£ظ† طھطµظ„ آ«ط¶آ»طں", font_size=36, color=WHITE, bold=True,
             alignment=PP_ALIGN.RIGHT)

add_section_line(slide, Inches(6), Inches(1.2), Inches(6.5))

add_text_box(slide, Inches(0.5), Inches(1.6), Inches(12.3), Inches(0.8),
             "ظ…ظ† ظ„ط؛ط© ط¹ط±ط¨ظٹط© ط¹ط§ظ„ظٹط© ط§ظ„ظ…ط³طھظˆظ‰\nط¥ظ„ظ‰ ظ„ط؛ط© طھظ…طھظ„ظƒ Compiler ظƒط§ظ…ظ„",
             font_size=22, color=GLASS_GREEN, bold=True, alignment=PP_ALIGN.CENTER)

vision_items = [
    ("Lexer", True),
    ("Parser", True),
    ("AST", True),
    ("Semantic Analysis", False),
    ("IR", False),
    ("Backend", False),
    ("Assembly", False),
    ("Machine Code", False),
]

box_w = Inches(1.35)
start_x = Inches(0.4)
y_vis = Inches(3.3)

for i, (text, is_done) in enumerate(vision_items):
    x = start_x + i * Inches(1.6)
    shape = add_box(slide, x, y_vis, box_w, Inches(0.65), text,
                    bg_color=GLASS_GREEN if is_done else CARD_BG,
                    text_color=WHITE, font_size=11, bold=True)
    if not is_done:
        shape.line.color.rgb = RGBColor(0x55, 0xAA, 0xFF)
        shape.line.width = Pt(1.5)
    status = "âœ“" if is_done else "â—¯"
    s_color = GLASS_GREEN if is_done else RGBColor(0x55, 0xAA, 0xFF)
    add_text_box(slide, x, y_vis + Inches(0.75), box_w, Inches(0.3),
                 status, font_size=14, color=s_color, alignment=PP_ALIGN.CENTER)
    if i < len(vision_items) - 1:
        add_arrow(slide, x + box_w, y_vis + Inches(0.18), Inches(0.2), Inches(0.25),
                  color=GLASS_GREEN_DARK)

add_text_box(slide, Inches(0.5), Inches(4.8), Inches(12.3), Inches(0.5),
             "âœ“ ظ…ظˆط¬ظˆط¯ ط­ط§ظ„ظٹط§ظ‹  آ·  â—¯ ظ…ط®ط·ط· ظ„ظ„ظ…ط³طھظ‚ط¨ظ„", font_size=14, color=MID_GRAY,
             alignment=PP_ALIGN.CENTER)

add_text_box(slide, Inches(0.5), Inches(5.5), Inches(12.3), Inches(1),
             "ط§ظ„ط±ط¤ظٹط©: ط¨ظ†ط§ط، ظ„ط؛ط© ط¨ط±ظ…ط¬ط© ط¹ط±ط¨ظٹط© ظ…طھظƒط§ظ…ظ„ط© طھظ…طھظ„ظƒ ظ…ط³ط§ط±ط§ظ‹ ظƒط§ظ…ظ„ط§ظ‹\nظ…ظ† ط§ظ„ظƒظˆط¯ ط§ظ„ط¹ط±ط¨ظٹ ط¥ظ„ظ‰ طھظ†ظپظٹط° ط¹ظ„ظ‰ ط§ظ„ظ…ط¹ط§ظ„ط¬ ظ…ط¨ط§ط´ط±ط©",
             font_size=16, color=LIGHT_GRAY, alignment=PP_ALIGN.CENTER)

add_slide_number(slide, 27, TOTAL_SLIDES)

# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
# Slide 28: Conclusion
# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
slide = prs.slides.add_slide(prs.slide_layouts[6])
add_bg(slide)

add_rect(slide, Inches(0), Inches(0), W, Inches(0.08), GLASS_GREEN)

add_text_box(slide, Inches(1), Inches(1.5), Inches(11.3), Inches(1),
             "ط§ظ„ط®ط§طھظ…ط©", font_size=48, color=WHITE, bold=True,
             alignment=PP_ALIGN.CENTER, font_name="Segoe UI Light")

summary = [
    ("آ«ط¶آ» ظ„ط؛ط© ط¨ط±ظ…ط¬ط© ط¹ط±ط¨ظٹط© ط¹ط§ظ„ظٹط© ط§ظ„ظ…ط³طھظˆظ‰", GLASS_GREEN),
    ("طھظ… ط¨ظ†ط§ط، ظ…طھط±ط¬ظ… ظƒط§ظ…ظ„ (Lexer â†’ AST â†’ C++20)", GLASS_GREEN),
    ("ط¨ظٹط¦ط© ظˆظٹط¨ طھظپط§ط¹ظ„ظٹط© + ظ…ظ†طµط© طھط¹ظ„ظٹظ…ظٹط© ظ…طھظƒط§ظ…ظ„ط©", GLASS_GREEN),
    ("طھطھط·ظ„ط¨ ط§ظ„ط·ط±ظٹظ‚ ط¥ظ„ظ‰ ط§ظ„ظ…ط³طھظˆظ‰ ط§ظ„ظ…ظ†ط®ظپط¶ ط¨ظ†ط§ط، Semantic Analysis + IR + Backend", GLASS_GREEN),
]

for i, (text, color) in enumerate(summary):
    y = Inches(2.5 + i * 1.0)
    card = add_rounded_rect(slide, Inches(1.5), y, Inches(10.3), Inches(0.8), CARD_BG)
    card.line.color.rgb = CARD_BORDER
    card.line.width = Pt(1)
    add_rect(slide, Inches(1.5), y, Inches(0.06), Inches(0.8), color)
    add_text_box(slide, Inches(1.8), y + Inches(0.15), Inches(9.7), Inches(0.5),
                 text, font_size=18, color=WHITE, alignment=PP_ALIGN.RIGHT)

add_text_box(slide, Inches(1), Inches(6.2), Inches(11.3), Inches(0.8),
             "آ«ط¶ â€” ظ…ظ† ط§ظ„ط¨ط±ظ…ط¬ط© ط¨ط§ظ„ط¹ط±ط¨ظٹط© ط¥ظ„ظ‰ ط¨ظ†ط§ط، ظ„ط؛ط© ظ…طھظƒط§ظ…ظ„ط©آ»",
             font_size=22, color=GLASS_GREEN, bold=True, alignment=PP_ALIGN.CENTER)

add_slide_number(slide, 28, TOTAL_SLIDES)

# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
# Slide 29: Sources
# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
slide = prs.slides.add_slide(prs.slide_layouts[6])
add_bg(slide)
add_top_bar(slide)

add_text_box(slide, Inches(0.8), Inches(0.5), Inches(11.7), Inches(0.8),
             "ظ…طµط§ط¯ط± ط§ظ„ظ…ط¹ظ„ظˆظ…ط§طھ", font_size=36, color=WHITE, bold=True,
             alignment=PP_ALIGN.RIGHT)

add_section_line(slide, Inches(7.5), Inches(1.2), Inches(5))

sources = [
    "PROJECT_LANGUAGE_REPORT.md â€” ط§ظ„طھظ‚ط±ظٹط± ط§ظ„ظ…ط±ط¬ط¹ظٹ ط§ظ„ط´ط§ظ…ظ„",
    "src/Lexer.cpp â€” ظ…ط­ظ„ظ„ ط§ظ„ظ„ظپط¸ظٹ (207 ط³ط·ط±)",
    "src/Parser.cpp â€” ظ…ط­ظ„ظ„ ط§ظ„ظ†ط­ظˆ (1378 ط³ط·ط±)",
    "src/CodeGen.cpp â€” ظ…ظˆظ„ظ‘ط¯ ط§ظ„ظƒظˆط¯ (640 ط³ط·ط±)",
    "src/Compiler.cpp â€” ظ…ظ†ط³ظ‘ظ‚ ط®ط· ط£ظ†ط§ط¨ظٹط¨ ط§ظ„طھط±ط¬ظ…ط©",
    "include/Daad/AST.hpp â€” طھط¹ط±ظٹظپ ط´ط¬ط±ط© ط§ظ„ظ†ط­ظˆ (691 ط³ط·ط±)",
    "include/Daad/Keywords.hpp â€” ط³ط¬ظ„ ط§ظ„ظƒظ„ظ…ط§طھ ط§ظ„ظ…ظپطھط§ط­ظٹط©",
    "tests/*.cpp â€” ط§ط®طھط¨ط§ط±ط§طھ GoogleTest (239طŒ طھط­ظ‚ظ‚ 2026-08-14)",
    "examples/*.ض â€” ط£ظ…ط«ظ„ط© ط­ظ‚ظٹظ‚ظٹط© ظ…ظ† ط§ظ„ظ…ط´ط±ظˆط¹",
    "frontend-web/js/dhad-*.js â€” ظ…ط­ظˆظ‘ظ„ ط§ظ„ظˆظٹط¨",
]

add_bullet_list(slide, Inches(0.5), Inches(1.8), Inches(12.3), Inches(5),
                sources, font_size=14, color=LIGHT_GRAY)

add_slide_number(slide, 29, TOTAL_SLIDES)

# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
# Save
# â•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گâ•گ
output_path = r"C:\Projects\daad-studio\DAAD_LANGUAGE_PRESENTATION_v2.pptx"
prs.save(output_path)
print(f"Saved: {output_path}")
print(f"Slides: {len(prs.slides)}")
