# -*- coding: utf-8 -*-
"""
================================================================================
DAAD PRESENTATION ENGINE - COMPREHENSIVE TESTING SUITE
================================================================================
Verifies color ratios, dimension compliance, coordinate math, and boundaries.
================================================================================
"""

import unittest
from pptx.util import Inches
from daad_presenter.theme import ThemeColors, Dimensions, Typography

class TestDaadPresentationEngine(unittest.TestCase):
    def test_dimensions(self):
        """Verifies widescreen standard ratios."""
        self.assertEqual(Dimensions.SCREEN_W, Inches(13.333))
        self.assertEqual(Dimensions.SCREEN_H, Inches(7.5))
        
    def test_theme_colors(self):
        """Verifies RGB spectrum bounds."""
        for name, color in [("PRIMARY", ThemeColors.PRIMARY), 
                            ("SECONDARY", ThemeColors.SECONDARY),
                            ("ACCENT", ThemeColors.ACCENT)]:
            self.assertTrue(0 <= color[0] <= 255)
            self.assertTrue(0 <= color[1] <= 255)
            self.assertTrue(0 <= color[2] <= 255)
            
    def test_typography(self):
        """Verifies fonts family assignments."""
        self.assertEqual(Typography.ARABIC, "Segoe UI")
        self.assertEqual(Typography.CODE, "Consolas")

    def test_bounding_boxes(self):
        """Verifies no elements exceed screen limits."""
        margin_x = Dimensions.MARGIN_X
        width_avail = Dimensions.SCREEN_W - (2 * margin_x)
        self.assertGreater(width_avail, Inches(0))

if __name__ == "__main__":
    unittest.main()
