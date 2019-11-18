# -*- coding: utf-8 -*-

class GildedRose(object):

    def __init__(self, items):
        self.items = items

    def update_quality(self):
         for item in self.items:

            if item.name == "Sulfuras, Hand of Ragnaros":
                item.quality = 80
                continue

            if item.name == "Aged Brie":
                self.increase_quality(item)   #Przydatne - Refactor->extract->method
                item.sell_in = item.sell_in - 1
                if item.sell_in < 0:
                    self.increase_quality(item)
                continue

            elif item.name == "Backstage passes to a TAFKAL80ETC concert":
                if item.sell_in < 6:
                    item.quality = item.quality +3
                elif item.sell_in < 11:
                    item.quality = item.quality +2
                else:
                    item.quality = item.quality +1

                item.sell_in = item.sell_in - 1
                if item.sell_in < 0:
                    item.quality = 0
                item.quality = min(item.quality, 50)
                continue

            else:
                item.quality = item.quality -1
                item.quality = max(item.quality, 0)  # Nigdy nie zejdzie pon 0

                item.sell_in= item.sell_in -1

                if item.sell_in < 0:
                    item.quality = item.quality - 1
                    item.quality = max(item.quality, 0)
                   
    def increase_quality(self, item):
        if item.quality < 50:
            item.quality = item.quality + 1
                

class Item:
    def __init__(self, name, sell_in, quality):
        self.name = name
        self.sell_in = sell_in
        self.quality = quality

    def __repr__(self):
        return "%s, %s, %s" % (self.name, self.sell_in, self.quality)
