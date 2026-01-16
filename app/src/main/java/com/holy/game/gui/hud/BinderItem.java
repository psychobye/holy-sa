package com.holy.game.gui.hud;

import java.io.Serializable;

public class BinderItem implements Serializable {
    String text;
    int color;

    public BinderItem(String text, int color) {
        this.text = text;
        this.color = color;
    }
}
