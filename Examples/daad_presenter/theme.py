# -*- coding: utf-8 -*-
"""
================================================================================
DAAD PRESENTATION ENGINE - THEME & LAYOUT MODULE
================================================================================
Defines design constants, color schemes, typography, and viewport parameters.
================================================================================
"""

from pptx.util import Inches, Pt
from pptx.dml.color import RGBColor

class ThemeColors:
    # Core Branding
    PRIMARY = RGBColor(0x00, 0xA8, 0x8A)      # Main Emerald Green
    SECONDARY = RGBColor(0x00, 0x7A, 0x63)    # Deep Forest Green
    ACCENT = RGBColor(0x00, 0xC9, 0xA7)       # Bright Mint Green
    
    # Backgrounds
    BG_CARD = RGBColor(0xF0, 0xFA, 0xF7)      # Soft Mint Backing
    BG_MAIN = RGBColor(0xFF, 0xFF, 0xFF)      # Pure White Background
    CODE_BG = RGBColor(0xF8, 0xFC, 0xFB)      # Soft IDE Screen Background
    
    # Text Shades
    TEXT_DARK = RGBColor(0x2C, 0x3E, 0x50)    # Dark Navy Slate
    TEXT_BODY = RGBColor(0x34, 0x49, 0x5E)    # Neutral Grayish Dark
    TEXT_MUTED = RGBColor(0x7F, 0x8C, 0x8D)   # Cool Muted Gray
    WHITE = RGBColor(0xFF, 0xFF, 0xFF)
    BORDER = RGBColor(0xBD, 0xC3, 0xC7)       # Silver Gray Border
    
    # Status Indicators
    STATUS_GREEN = RGBColor(0x00, 0xA8, 0x8A)
    STATUS_ORANGE = RGBColor(0xE6, 0x7E, 0x22)
    STATUS_BLUE = RGBColor(0x34, 0x98, 0xDB)
    STATUS_RED = RGBColor(0xE7, 0x4C, 0x3C)
    
    # Extended Vibrant Text Palette (for richer multi-color typography)
    TEXT_PURPLE = RGBColor(0x7A, 0x3E, 0xB1)  # Deep Purple
    TEXT_BLUE = RGBColor(0x1F, 0x6F, 0xC0)    # Rich Blue
    TEXT_TEAL = RGBColor(0x0F, 0x8A, 0x9E)    # Deep Teal
    TEXT_GOLD = RGBColor(0xB0, 0x8D, 0x1E)    # Golden Amber
    TEXT_CORAL = RGBColor(0xD0, 0x4B, 0x4B)   # Coral Red
    TEXT_INDIGO = RGBColor(0x3F, 0x51, 0xB5)  # Indigo
    TEXT_BROWN = RGBColor(0x8D, 0x5A, 0x2B)   # Chestnut Brown
    TEXT_PINK = RGBColor(0xC2, 0x18, 0x5B)    # Magenta Pink

    # Ready-to-cycle palette for card titles (RTL-friendly strong hues)
    TITLE_PALETTE = [
        PRIMARY,
        TEXT_BLUE,
        TEXT_PURPLE,
        TEXT_GOLD,
        TEXT_CORAL,
        TEXT_TEAL,
        TEXT_INDIGO,
        TEXT_PINK,
    ]

    @staticmethod
    def cycle(idx):
        """Cycles the palette safely for any index count."""
        palette = ThemeColors.TITLE_PALETTE
        return palette[idx % len(palette)]

class Typography:
    ARABIC = "Segoe UI"
    CODE = "Consolas"

class Dimensions:
    SCREEN_W = Inches(13.333)
    SCREEN_H = Inches(7.5)
    HEADER_H = Inches(1.0)
    MARGIN_X = Inches(0.5)
    MARGIN_Y = Inches(0.5)
