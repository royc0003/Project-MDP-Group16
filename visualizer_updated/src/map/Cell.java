package map;

/**
 * Represents each cell in the map grid.

 */

public class Cell {
    private final int row;
    private final int col;
    private boolean isObstacle;
    private boolean isVirtualWall;
    private boolean isExplored;
    /*
     * This would set every counter to 0
     * */
    private int reward = 0;
    private boolean isRewarded = true;

    public Cell(int row, int col) {
        this.row = row;
        this.col = col;
    }

    public int getRow() {
        return this.row;
    }

    public int getCol() {
        return this.col;
    }

    public void setIsObstacle(boolean val) {
        this.isObstacle = val;
    }

    public boolean getIsObstacle() {
        return this.isObstacle;
    }

    public void setVirtualWall(boolean val) {
        if (val) {
            this.isVirtualWall = true;
        } else {
            if (row != 0 && row != MapConstants.MAP_ROWS - 1 && col != 0 && col != MapConstants.MAP_COLS - 1) {
                this.isVirtualWall = false;
            }
        }
    }

    public boolean getIsVirtualWall() {
        return this.isVirtualWall;
    }

    public void setIsExplored(boolean val) {
        this.isExplored = val;
    }

    public boolean getIsExplored() {
        return this.isExplored;
    }
/*
-Reward base system
* */
    public int getReward() {
        return reward;
    }

    public void setReward(int reward) {
        this.reward = reward;
    }

    public void increaseReward() {
        this.reward += 1;
    }

    public void decreaseReward() {
        this.reward -= 1;
    }
    public void giveBonusReward() {
        this.reward += 2;
    }

    public void givePenaltyReward() {
        this.reward -= 1;
    }
    public boolean isRewarded(){
        if(this.reward >0){
            return this.isRewarded;
        }
        return !this.isRewarded;
    }

}