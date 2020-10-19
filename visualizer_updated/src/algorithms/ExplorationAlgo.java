package algorithms;

import java.util.*;

import map.Cell;
import map.Map;
import map.MapConstants;
import robot.Robot;
import robot.RobotConstants;
import robot.RobotConstants.DIRECTION;
import robot.RobotConstants.MOVEMENT;
import utils.CommMgr;



public class ExplorationAlgo {
    private final Map exploredMap;
    private final Map realMap;
    private final Robot bot;
    private final int coverageLimit;
    private final int timeLimit;
    private int areaExplored;
    private long startTime;
    private long endTime;
    private int lastFrontCalibrate = 0;
    private boolean calibrationMode;
    private DIRECTION prevDir = DIRECTION.NORTH;
    private ArrayList<MOVEMENT> movement = new ArrayList<MOVEMENT>();
    private int prevLocRow;
    private int prevLocCol;

    public ExplorationAlgo(Map exploredMap, Map realMap, Robot bot, int coverageLimit, int timeLimit) {
        this.exploredMap = exploredMap;
        this.realMap = realMap;
        this.bot = bot;
        this.coverageLimit = coverageLimit;
        this.timeLimit = timeLimit;
    }

    /**
     * Main method that is called to start the exploration.
     */
    public void runExploration() {
        if (bot.getRealBot()) {
            System.out.println("Starting calibration...");

            CommMgr.getCommMgr().recvMsg();
            if (bot.getRealBot()) {
                CommMgr.getCommMgr().sendMsg(null, CommMgr.CALIBRATE);
            }
            while(true){
                System.out.println("Waiting for Calibration_Ok...");
                String msg = CommMgr.getCommMgr().recvMsg();
                if (msg.equals("k")) break;
            }

            while (true) {
                System.out.println("Waiting for EX_START...");
                String msg = CommMgr.getCommMgr().recvMsg();
                if (msg.equals(CommMgr.EX_START)) break;
            }
        }

        System.out.println("Starting exploration...");

        startTime = System.currentTimeMillis();
        endTime = startTime + (timeLimit * 1000);

        if (bot.getRealBot()) {
            CommMgr.getCommMgr().sendMsg(null, CommMgr.BOT_START);
        }
        senseAndRepaint();

        areaExplored = calculateAreaExplored();
        System.out.println("Explored Area: " + areaExplored);

        explorationLoop(bot.getRobotPosRow(), bot.getRobotPosCol());
    }

    /**
     * Loops through robot movements until one (or more) of the following conditions is met:
     * 1. Robot is back at (r, c)
     * 2. areaExplored > coverageLimit
     * 3. System.currentTimeMillis() > endTime
     */
    private void explorationLoop(int r, int c) {
        do {
            nextMove();

            areaExplored = calculateAreaExplored();
            System.out.println("Area explored: " + areaExplored);

            if (bot.getRobotPosRow() == r && bot.getRobotPosCol() == c) {
                if (areaExplored >= 100) {
                    break;
                }
            }
        } while (areaExplored <= coverageLimit && System.currentTimeMillis() <= endTime);

        goHome();
    }

    /**
     * Determines the next move for the robot and executes it accordingly.
     */
    private void nextMove() {
        if (lookRight()) {
            movement.add(MOVEMENT.RIGHT);
            moveBot(MOVEMENT.RIGHT);
            if (lookForward()) {
                movement.add(MOVEMENT.FORWARD);
                moveBot(MOVEMENT.FORWARD);
            }
        } else if (lookForward()) {
            movement.add(MOVEMENT.FORWARD);
            moveBot(MOVEMENT.FORWARD);
        } else if (lookLeft()) {
            movement.add(MOVEMENT.LEFT);
            moveBot(MOVEMENT.LEFT);
            if (lookForward()) {
                movement.add(MOVEMENT.FORWARD);
                moveBot(MOVEMENT.FORWARD);
            }
        }
        else {
            movement.add(MOVEMENT.RIGHT);
            movement.add(MOVEMENT.RIGHT);
            moveBot(MOVEMENT.RIGHT);
            moveBot(MOVEMENT.RIGHT);
        }
    }

    /**
     * Returns true if the right side of the robot is free to move into.
     */
    private boolean lookRight() {
        switch (bot.getRobotCurDir()) {
            case NORTH:
                return eastFree();
            case EAST:
                return southFree();
            case SOUTH:
                return westFree();
            case WEST:
                return northFree();
        }
        return false;
    }

    /**
     * Returns true if the robot is free to move forward.
     */
    private boolean lookForward() {
        switch (bot.getRobotCurDir()) {
            case NORTH:
                return northFree();
            case EAST:
                return eastFree();
            case SOUTH:
                return southFree();
            case WEST:
                return westFree();
        }
        return false;
    }

    /**
     * * Returns true if the left side of the robot is free to move into.
     */
    private boolean lookLeft() {
        switch (bot.getRobotCurDir()) {
            case NORTH:
                return westFree();
            case EAST:
                return northFree();
            case SOUTH:
                return eastFree();
            case WEST:
                return southFree();
        }
        return false;
    }

    /**
     * Returns true if the robot can move to the north cell.
     */
    private boolean northFree() {
        int botRow = bot.getRobotPosRow();
        int botCol = bot.getRobotPosCol();
        return (isExploredNotObstacle(botRow + 1, botCol - 1) && isExploredAndFree(botRow + 1, botCol) && isExploredNotObstacle(botRow + 1, botCol + 1));
    }

    /**
     * Returns true if the robot can move to the east cell.
     */
    private boolean eastFree() {
        int botRow = bot.getRobotPosRow();
        int botCol = bot.getRobotPosCol();
        return (isExploredNotObstacle(botRow - 1, botCol + 1) && isExploredAndFree(botRow, botCol + 1) && isExploredNotObstacle(botRow + 1, botCol + 1));
    }

    /**
     * Returns true if the robot can move to the south cell.
     */
    private boolean southFree() {
        int botRow = bot.getRobotPosRow();
        int botCol = bot.getRobotPosCol();
        return (isExploredNotObstacle(botRow - 1, botCol - 1) && isExploredAndFree(botRow - 1, botCol) && isExploredNotObstacle(botRow - 1, botCol + 1));
    }

    /**
     * Returns true if the robot can move to the west cell.
     */
    private boolean westFree() {
        int botRow = bot.getRobotPosRow();
        int botCol = bot.getRobotPosCol();
        return (isExploredNotObstacle(botRow - 1, botCol - 1) && isExploredAndFree(botRow, botCol - 1) && isExploredNotObstacle(botRow + 1, botCol - 1));
    }

    /**
     * Returns the robot to START after exploration and points the bot northwards.
     */
    private void goHome() {
        if (!bot.getTouchedGoal() && coverageLimit == 300 && timeLimit == 3600) {
            FastestPathAlgo goToGoal = new FastestPathAlgo(exploredMap, bot, realMap);
            goToGoal.runFastestPath(RobotConstants.GOAL_ROW, RobotConstants.GOAL_COL);
        }

        FastestPathAlgo returnToStart = new FastestPathAlgo(exploredMap, bot, realMap);
        returnToStart.runFastestPath(RobotConstants.START_ROW, RobotConstants.START_COL);

        System.out.println("Exploration complete!");
        areaExplored = calculateAreaExplored();
        System.out.printf("%.2f%% Coverage", (areaExplored / 300.0) * 100.0);
        System.out.println(", " + areaExplored + " Cells");
        System.out.println((System.currentTimeMillis() - startTime) / 1000 + " Seconds");

        /**
         * Enter start zone finish timing
         */

        CommMgr comm = CommMgr.getCommMgr();
        comm.sendMsg("DONE", CommMgr.DONE_EX);

//        if (bot.getRealBot()) {
//            turnBotDirection(DIRECTION.WEST);
//            turnBotDirection(DIRECTION.SOUTH);
//            turnBotDirection(DIRECTION.WEST);
//        }
        turnBotDirection(DIRECTION.NORTH);
        CommMgr.getCommMgr().sendMsg(null, CommMgr.BOT_START);
    }


    /**
     *
     Anti stuck
     */

    public void antiStuck(){
        if(!lookRight()){
            System.out.println("I'm in look right");
            this.prevLocRow = bot.getRobotPosRow();
            this.prevLocCol = bot.getRobotPosCol();

            if (prevLocRow == 1)
                prevDir = DIRECTION.SOUTH;
            else if (prevLocRow == 13)
                prevDir = DIRECTION.NORTH;
            else if (prevLocCol == 1)
                prevDir = DIRECTION.EAST;
            else if (prevLocCol == 18)
                prevDir = DIRECTION.WEST;
        }

        if (stuckInLoop()) {
            //A*
            System.out.println(Arrays.toString(movement.toArray()));
            System.out.println("Prev loc: "+this.prevLocRow+"   "+this.prevLocCol);
            System.out.println("Running A* to go back...");
            FastestPathAlgo goToUnstuckLocation = new FastestPathAlgo(exploredMap, bot, realMap);
            goToUnstuckLocation .runFastestPath(this.prevLocRow, this.prevLocCol);
            turnBotDirection(this.prevDir);
            movement.clear();
        }
    }

    public boolean stuckInLoop(){
        if (movement.size() >= 4)
            if (movement.get(movement.size() - 1) == MOVEMENT.FORWARD &&
                    movement.get(movement.size() - 2) == MOVEMENT.RIGHT &&
                    movement.get(movement.size() - 3) == MOVEMENT.FORWARD &&
                    movement.get(movement.size() - 4) == MOVEMENT.RIGHT)
                return true;
        return false;
    }

    /**
     * Returns true for cells that are explored and not obstacles.
     */
    private boolean isExploredNotObstacle(int r, int c) {
        if (exploredMap.checkValidCoordinates(r, c)) {
            Cell tmp = exploredMap.getCell(r, c);
            return (tmp.getIsExplored() && !tmp.getIsObstacle());
        }
        return false;
    }

    /**
     * Returns true for cells that are explored, not virtual walls and not obstacles.
     */
    private boolean isExploredAndFree(int r, int c) {
        if (exploredMap.checkValidCoordinates(r, c)) {
            Cell b = exploredMap.getCell(r, c);
            return (b.getIsExplored() && !b.getIsVirtualWall() && !b.getIsObstacle());
        }
        return false;
    }

    /**
     * Returns the number of cells explored in the grid.
     */
    private int calculateAreaExplored() {
        int result = 0;
        for (int r = 0; r < MapConstants.MAP_ROWS; r++) {
            for (int c = 0; c < MapConstants.MAP_COLS; c++) {
                if (exploredMap.getCell(r, c).getIsExplored()) {
                    result++;
                }
            }
        }
        return result;
    }

    /**
     * Moves the bot, repaints the map and calls senseAndRepaint().
     * canSense == on Calibration don't paint
     */
    private void moveBot(MOVEMENT m) {
        bot.move(m);
        exploredMap.repaint(); //fixed 7th october
//        capturePhoto();
        if (m != MOVEMENT.CALIBRATE_RIGHT && m!= MOVEMENT.CALIBRATE_DISTANCE
                && m!= MOVEMENT.CALIBRATE_ANGLE_LR && m!= MOVEMENT.CALIBRATE_ANGLE_LC
                && m!= MOVEMENT.CALIBRATE_ANGLE_RC ){
            senseAndRepaint();
        }
        else {
            CommMgr commMgr = CommMgr.getCommMgr();
            commMgr.recvMsg();
        }

        if (bot.getRealBot() && !calibrationMode) {
            calibrationMode = true;
            rightCalibrate();
            frontCalibrate();
//            cornerCalibrate();
//            lastFrontCalibrate++;
//            if(lastCalibrate >= 5){
//                normalCalibrate();
//            } else {
//                rightCalibrate();
//                lastCalibrate++;
//            }
//            if (lastFrontCalibrate >= 5) {
//                DIRECTION targetDir = getCalibrationDirection();
//                if (targetDir != null) {
//                    lastFrontCalibrate = 0;
//                    DIRECTION origDir = bot.getRobotCurDir();
//                    turnBotDirection(targetDir);
//                    frontCalibrate();
//                    turnBotDirection(origDir);
//                }
//            }
            calibrationMode = false;
        }
    }

    /** At the 3rd movement, if possible do right calibration first, then distance alibration */
//    private void normalCalibrate(){
//        rightCalibrate();
//        MOVEMENT m = canCalibrateOnTheSpotFront(bot.getRobotCurDir());
//        if(m != null){
//            distanceCalibrate(bot.getRobotCurDir());
//        } else {
//            DIRECTION dirToCheck = DIRECTION.getNext(bot.getRobotCurDir());
//            distanceCalibrate(dirToCheck);
//        }
//    }

    /** This calibration is activated (distance both) when meet with a right angle. */
    private void cornerCalibrate(){
        DIRECTION origDir = bot.getRobotCurDir();
        DIRECTION dirToCheck = DIRECTION.getNext(bot.getRobotCurDir());

        MOVEMENT m = canCalibrateOnTheSpotFront(dirToCheck);
        if(canCalibrateOnTheSpotFront(origDir) != null){
            if(m == MOVEMENT.CALIBRATE_ANGLE_LR || m == MOVEMENT.CALIBRATE_ANGLE_RC){
                turnBotDirection(dirToCheck);
                moveBot(m);
                moveBot(MOVEMENT.CALIBRATE_DISTANCE);
                turnBotDirection(origDir);
                rightCalibrate();
            } else if(m == MOVEMENT.CALIBRATE_ANGLE_LC){
                turnBotDirection(dirToCheck);
                moveBot(m);
                turnBotDirection(origDir);
                rightCalibrate();

            }

        }

//        distanceCalibrate(dirToCheck); //check can calibrate right at corner first
//        distanceCalibrate(origDir); // check can calibrate front at corner next
    }

    /** Right calibration */
    private void rightCalibrate(){
        if(canCalibrateOnTheSpotRight(bot.getRobotCurDir()) && bot.checkRightNotPhantom()){
            moveBot(MOVEMENT.CALIBRATE_RIGHT);
//            lastCalibrate = 0;
        }
    }

    /** Front calibration*/
    private void frontCalibrate(){
        MOVEMENT m = canCalibrateOnTheSpotFront(bot.getRobotCurDir());
        if(m == MOVEMENT.CALIBRATE_ANGLE_LR || m == MOVEMENT.CALIBRATE_ANGLE_RC){
            moveBot(m);
            moveBot(MOVEMENT.CALIBRATE_DISTANCE);
        } else if(m == MOVEMENT.CALIBRATE_ANGLE_LC){
            moveBot(m);
        }
    }

    /** Distance calibrate */
//    private void distanceCalibrate(DIRECTION dirToCheck){
//        DIRECTION origDir = bot.getRobotCurDir();
//        MOVEMENT calibrate = canCalibrateOnTheSpotFront(dirToCheck);
//        if(calibrate != null) {
//            turnBotDirection(dirToCheck);
//            moveBot(calibrate);
//            turnBotDirection(origDir);
//            lastCalibrate = 0;
//        }
//    }

    /**
     * Returns a possible direction for robot calibration or null, otherwise.
     */
    private DIRECTION getCalibrationDirection() {
        DIRECTION origDir = bot.getRobotCurDir();
        DIRECTION dirToCheck;

        dirToCheck = DIRECTION.getNext(origDir);                    // right turn
        if (canCalibrateOnTheSpotFront(dirToCheck) != null) return dirToCheck;

        dirToCheck = DIRECTION.getPrevious(origDir);                // left turn
        if (canCalibrateOnTheSpotFront(dirToCheck) != null) return dirToCheck;

        dirToCheck = DIRECTION.getPrevious(dirToCheck);             // u turn
        if (canCalibrateOnTheSpotFront(dirToCheck) != null) return dirToCheck;

        return null;
    }

    /**
     * Sets the bot's sensors, processes the sensor data and repaints the map.
     */
    private void senseAndRepaint() {
        System.out.println("Sensing and repainting");
        bot.setSensors();
        bot.sense(exploredMap, realMap);
        exploredMap.repaint();
    }

    private void capturePhoto(){
        System.out.println("Receiving photo");
        CommMgr comm = CommMgr.getCommMgr();
        String msg = comm.recvMsg();
        System.out.println(msg);
    }

//    public MOVEMENT canCalibrateOnTheSpot(DIRECTION dirToCheck){
//        MOVEMENT m1 = canCalibrateOnTheSpotFront(dirToCheck);
//        if(m1 != null) return m1;
//        if(canCalibrateOnTheSpotRight(dirToCheck)) return MOVEMENT.CALIBRATE_RIGHT;
//        return null;
//    }

    public MOVEMENT canCalibrateOnTheSpotFront(DIRECTION dirToCheck){
        if(canCalibrateOnTheSpotFrontLeft_Right(dirToCheck)) return MOVEMENT.CALIBRATE_ANGLE_LR;
         else if(canCalibrateOnTheSpotFrontLeft_Center(dirToCheck)) return MOVEMENT.CALIBRATE_ANGLE_LC;
         else if(canCalibrateOnTheSpotFrontRight_Center(dirToCheck)) return MOVEMENT.CALIBRATE_ANGLE_RC;
        return null;
    }

    /**
     * Checks if the robot can calibrate at its current position given a direction.
     */
    private boolean canCalibrateOnTheSpotRight(DIRECTION botDir) {
        int row = bot.getRobotPosRow();
        int col = bot.getRobotPosCol();

        switch (botDir) {
            case NORTH:
                return exploredMap.getIsObstacleOrWall(row + 1, col + 2) && exploredMap.getIsObstacleOrWall(row - 1, col + 2);
            case EAST:
                return exploredMap.getIsObstacleOrWall(row - 2, col + 1) && exploredMap.getIsObstacleOrWall(row - 2 , col - 1);
            case SOUTH:
                return exploredMap.getIsObstacleOrWall(row - 1, col - 2) && exploredMap.getIsObstacleOrWall(row + 1, col - 2);
            case WEST:
                return exploredMap.getIsObstacleOrWall(row + 2, col - 1) && exploredMap.getIsObstacleOrWall(row + 2, col + 1);
        }
        return false;
    }

    public boolean canCalibrateOnTheSpotFrontLeft_Right(DIRECTION botDir){
        int row = bot.getRobotPosRow();
        int col = bot.getRobotPosCol();
        switch (botDir) {
            case NORTH:
                return exploredMap.getIsObstacleOrWall(row + 2, col - 1) && exploredMap.getIsObstacleOrWall(row + 2, col + 1);
            case EAST:
                return exploredMap.getIsObstacleOrWall(row + 1, col + 2) && exploredMap.getIsObstacleOrWall(row - 1, col + 2);
            case SOUTH:
                return exploredMap.getIsObstacleOrWall(row - 2, col - 1) && exploredMap.getIsObstacleOrWall(row - 2, col + 1);
            case WEST:
                return exploredMap.getIsObstacleOrWall(row + 1, col - 2) && exploredMap.getIsObstacleOrWall(row - 1, col - 2);
        }

        return false;
    }
    public boolean canCalibrateOnTheSpotFrontLeft_Center(DIRECTION botDir){
        int row = bot.getRobotPosRow();
        int col = bot.getRobotPosCol();
        switch (botDir) {
            case NORTH:
                return exploredMap.getIsObstacleOrWall(row + 2, col - 1) && exploredMap.getIsObstacleOrWall(row + 2, col);
            case EAST:
                return exploredMap.getIsObstacleOrWall(row + 1, col + 2) && exploredMap.getIsObstacleOrWall(row, col + 2);
            case SOUTH:
                return exploredMap.getIsObstacleOrWall(row - 2, col - 1) && exploredMap.getIsObstacleOrWall(row - 2, col);
            case WEST:
                return exploredMap.getIsObstacleOrWall(row + 1, col - 2) && exploredMap.getIsObstacleOrWall(row, col - 2);
        }

        return false;
    }

    public boolean canCalibrateOnTheSpotFrontRight_Center(DIRECTION botDir){
        int row = bot.getRobotPosRow();
        int col = bot.getRobotPosCol();
        switch (botDir) {
            case NORTH:
                return exploredMap.getIsObstacleOrWall(row + 2, col) && exploredMap.getIsObstacleOrWall(row + 2, col + 1);
            case EAST:
                return exploredMap.getIsObstacleOrWall(row, col + 2) && exploredMap.getIsObstacleOrWall(row - 1, col + 2);
            case SOUTH:
                return exploredMap.getIsObstacleOrWall(row - 2, col) && exploredMap.getIsObstacleOrWall(row - 2, col + 1);
            case WEST:
                return exploredMap.getIsObstacleOrWall(row, col - 2) && exploredMap.getIsObstacleOrWall(row - 1, col - 2);
        }

        return false;
    }

    /**
     * Turns the robot to the required direction.
     */
    private void turnBotDirection(DIRECTION targetDir) {
        int numOfTurn = Math.abs(bot.getRobotCurDir().ordinal() - targetDir.ordinal());
        if (numOfTurn > 2) numOfTurn = numOfTurn % 2;

        if (numOfTurn == 1) {
            if (DIRECTION.getNext(bot.getRobotCurDir()) == targetDir) {
                moveBot(MOVEMENT.RIGHT);
            } else {
                moveBot(MOVEMENT.LEFT);
            }
        } else if (numOfTurn == 2) {
            moveBot(MOVEMENT.RIGHT);
            moveBot(MOVEMENT.RIGHT);
        }
    }
}