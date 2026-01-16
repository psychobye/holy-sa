package com.holy.game.gui.tab;

public class PlayerData {
    int mID;
    int mLevel;
    String mName;
    int mPing;
    int color;

    public PlayerData(int id, int color, String name, int level, int ping) {
        this.mID = id;
        this.mName = name;
        this.mLevel = level;
        this.mPing = ping;
        this.color = color;
    }

    public int getId() {
        return this.mID;
    }

    public String getName() {
        return this.mName;
    }

    public int getPing() {
        return this.mPing;
    }

    public int getLevel() {
        return this.mLevel;
    }

    public void setId(int id) {
        this.mID = id;
    }

    public void setName(String name) {
        this.mName = name;
    }

    public void setPing(int ping) {
        this.mPing = ping;
    }

    public void setLevel(int level) {
        this.mLevel = level;
    }
}
