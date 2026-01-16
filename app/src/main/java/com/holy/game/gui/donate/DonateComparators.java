package com.holy.game.gui.donate;

import java.util.Comparator;

public class DonateComparators {

    public static class DonateComparatorByPrice implements Comparator<DonateItem> {
        @Override
        public int compare(DonateItem o1, DonateItem o2) {
            return Integer.compare(o1.getPrice(), o2.getPrice());
        }
    }
}
