# -*- coding: utf-8 -*-
import unittest

from gilded_rose import Item, GildedRose

class GildedRoseTest(unittest.TestCase):
    def test_foo(self):
        items = [Item("foo", 0, 0)]
        gilded_rose = GildedRose(items)
        gilded_rose.update_quality()
        self.assertEqual("foo", items[0].name)

    def test_sulfuras(self):
        items = [Item("Sulfuras, Hand of Ragnaros",20,80)]
        gilded_rose = GildedRose(items)
        gilded_rose.update_quality()
        self.assertEqual(80,items[0].quality)

    def test_sulfuras_past_date(self):
        items = [Item("Sulfuras, Hand of Ragnaros",0,80)]
        gilded_rose = GildedRose(items)
        gilded_rose.update_quality()
        self.assertEqual(80,items[0].quality)

if __name__ == '__main__':
    unittest.main()