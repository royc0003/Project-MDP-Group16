package robot;

import map.Map;
import map.MapConstants;
import robot.RobotConstants.DIRECTION;
import robot.RobotConstants.MOVEMENT;
import utils.CommMgr;
import utils.MapDescriptor;

import java.util.concurrent.TimeUnit;
import java.util.*;

// @formatter:off
/**
 * 
 *          ^   ^   ^
 *         SR  SR  SR
 *  
 *   < LR [X] [X] [X] SR >
 *        [X] [X] [X] 
 *        [X] [X] [X] SR >
 *
 * (Long range on top )
 *
 * SR = Short Range Sensor, LR = Long Range Sensor
 *
 */
// @formatter:on

public class Robot {
    private int posRow; // center cell
    private int posCol; // center cell
    private DIRECTION robotDir;
    private int speed;
    private final Sensor SRFrontLeft;       // north-facing front-left SR
    private final Sensor SRFrontCenter;     // north-facing front-center SR
    private final Sensor SRFrontRight;      // north-facing front-right SR
    private final Sensor SRRightTop;        // east-facing right SR
    private final Sensor SRRightBottom;     //NOTE: This is for SRRight-Bottom             // west-facing left SR    
    private final Sensor LRLeft;            // west-facing left LR
    private boolean touchedGoal;
    private final boolean realBot;
    private String explorationMapString;

    public Robot(int row, int col, boolean realBot) {
        posRow = row;
        posCol = col;
        robotDir = RobotConstants.START_DIR;
        speed = RobotConstants.SPEED;

        this.realBot = realBot;

        SRFrontLeft = new Sensor(RobotConstants.SENSOR_SHORT_RANGE_L, RobotConstants.SENSOR_SHORT_RANGE_H, this.posRow + 1, this.posCol - 1, this.robotDir, "SRFL");
        SRFrontCenter = new Sensor(RobotConstants.SENSOR_SHORT_RANGE_L, RobotConstants.SENSOR_SHORT_RANGE_H, this.posRow + 1, this.posCol, this.robotDir, "SRFC");
        SRFrontRight = new Sensor(RobotConstants.SENSOR_SHORT_RANGE_L, RobotConstants.SENSOR_SHORT_RANGE_H, this.posRow + 1, this.posCol + 1, this.robotDir, "SRFR");
        SRRightTop = new Sensor(RobotConstants.SENSOR_SHORT_RANGE_L, RobotConstants.SENSOR_SHORT_RANGE_H, this.posRow + 1, this.posCol + 1, findNewDirection(MOVEMENT.RIGHT), "SRRT");
        SRRightBottom = new Sensor(RobotConstants.SENSOR_SHORT_RANGE_L, RobotConstants.SENSOR_SHORT_RANGE_H, this.posRow - 1, this.posCol + 1, findNewDirection(MOVEMENT.RIGHT), "SRRB");
        LRLeft = new Sensor(RobotConstants.SENSOR_LONG_RANGE_L, RobotConstants.SENSOR_LONG_RANGE_H, this.posRow + 1, this.posCol - 1, findNewDirection(MOVEMENT.LEFT), "LRL");
    }

    public void setRobotPos(int row, int col) {
        posRow = row;
        posCol = col;
    }

    public int getRobotPosRow() {
        return posRow;
    }

    public int getRobotPosCol() {
        return posCol;
    }

    public void setRobotDir(DIRECTION dir) {
        robotDir = dir;
    }

    public void setSpeed(int speed) {
        this.speed = speed;
    }

    public DIRECTION getRobotCurDir() {
        return robotDir;
    }

    public boolean checkRightNotPhantom(){
        return SRRightTop.getRealSensorValue() == 1 && SRRightBottom.getRealSensorValue() == 1;
    }

    public char getCameraDirection(DIRECTION d){
        switch(d){
            case NORTH:
                return 'R';
            case EAST:
                return 'D';
            case SOUTH:
                return 'L';
            case WEST:
                return 'U';
        }
        return '-';
    }
    public char getAndroidCurDir(DIRECTION d){
        switch(d){
            case NORTH:
                return 'U';
            case EAST:
                return 'R';
            case SOUTH:
                return 'D';
            case WEST:
                return 'L';
        }
        return '-';
    }

    public boolean getRealBot() {
        return realBot;
    }

    private void updateTouchedGoal() {
        if (this.getRobotPosRow() == MapConstants.GOAL_ROW && this.getRobotPosCol() == MapConstants.GOAL_COL)
            this.touchedGoal = true;
    }

    public boolean getTouchedGoal() {
        return this.touchedGoal;
    }

    /**
     * Takes in a MOVEMENT and moves the robot accordingly by changing its position and direction. Sends the movement
     * if this.realBot is set.
     */
    public void move(MOVEMENT m, boolean sendMoveToAndroid) {
        if (!realBot) {
            // Emulate real movement by pausing execution.
            try {
                TimeUnit.MILLISECONDS.sleep(speed);
            } catch (InterruptedException e) {
                System.out.println("Something went wrong in Robot.move()!");
            }
        }
        switch (m) {
            case FORWARD:
                switch (robotDir) {
                    case NORTH:
                        posRow++;
                        break;
                    case EAST:
                        posCol++;
                        break;
                    case SOUTH:
                        posRow--;
                        break;
                    case WEST:
                        posCol--;
                        break;
                }
                break;
            case BACKWARD:
                switch (robotDir) {
                    case NORTH:
                        posRow--;
                        break;
                    case EAST:
                        posCol--;
                        break;
                    case SOUTH:
                        posRow++;
                        break;
                    case WEST:
                        posCol++;
                        break;
                }
                break;
            case RIGHT:
            case LEFT:
                robotDir = findNewDirection(m);
                break;
            case CALIBRATE_DISTANCE_LC:
            case CALIBRATE_DISTANCE_LR:
            case CALIBRATE_DISTANCE_RC:
            case CALIBRATE_RIGHT:
                break;
            default:
                System.out.println("Error in Robot.move()!");
                break;
        }

        if (realBot) sendMovement(m, sendMoveToAndroid); // this is to adrunio
        else System.out.println("Move: " + MOVEMENT.print(m));

        updateTouchedGoal();
    }

    /**
     * Overloaded method that calls this.move(MOVEMENT m, boolean sendMoveToAndroid = true).
     */
    public void move(MOVEMENT m) {
        this.move(m, true);
    }

    /**
     * Sends a number instead of 'F' for multiple continuous forward movements.
     */
    public void moveForwardMultiple(int count) {
        if (count == 1) {
            move(MOVEMENT.FORWARD);
        } else {
            CommMgr comm = CommMgr.getCommMgr();
            if (count == 10) {
                comm.sendMsg("0", CommMgr.INSTRUCTIONS);
            } else if (count < 10) {
                comm.sendMsg(Integer.toString(count), CommMgr.INSTRUCTIONS);
            }

            switch (robotDir) {
                case NORTH:
                    posRow += count;
                    break;
                case EAST:
                    posCol += count;
                    break;
                case SOUTH:
                    posRow += count;
                    break;
                case WEST:
                    posCol += count;
                    break;
            }

            //comm.sendMsg(this.getRobotPosRow() + "," + this.getRobotPosCol() + "," + DIRECTION.print(this.getRobotCurDir()), CommMgr.BOT_POS);
            // comm.sendMsg(this.getRobotPosCol() + "," + this.getRobotPosRow() + "," + DIRECTION.print(this.getRobotCurDir()), CommMgr.BOT_POS);
        }
    }

    /**
     * Uses the CommMgr to send the next movement to the robot.
     */
    private void sendMovement(MOVEMENT m, boolean sendMoveToAndroid) {
        CommMgr comm = CommMgr.getCommMgr();
        comm.sendMsg(MOVEMENT.print(m) + "", CommMgr.INSTRUCTIONS);
            String stringToSend = "EX|"+ this.explorationMapString+ "["+this.getRobotPosCol()+","+this.getRobotPosRow()+"]"+ "|" + getAndroidCurDir(this.getRobotCurDir());
            comm.sendMsg(stringToSend, CommMgr.TO_ANDROID);
            // String imgToSend = "|["+this.getRobotPosCol()+","+this.getRobotPosRow()+"]|"+getCameraDirection(this.getRobotCurDir())+"|";
            // comm.sendMsg(imgToSend, CommMgr.CAMERA);
    }

    /**
     * Sets the sensors' position and direction values according to the robot's current position and direction.
     */
    public void setSensors() {
        switch (robotDir) {
            case NORTH:
                SRFrontLeft.setSensor(this.posRow + 1, this.posCol - 1, this.robotDir);
                SRFrontCenter.setSensor(this.posRow + 1, this.posCol, this.robotDir);
                SRFrontRight.setSensor(this.posRow + 1, this.posCol + 1, this.robotDir);
                SRRightTop.setSensor(this.posRow + 1, this.posCol + 1, findNewDirection(MOVEMENT.RIGHT));
                SRRightBottom.setSensor(this.posRow - 1, this.posCol + 1, findNewDirection(MOVEMENT.RIGHT));
                LRLeft.setSensor(this.posRow + 1, this.posCol - 1, findNewDirection(MOVEMENT.LEFT));
                break;
            case EAST:
                SRFrontLeft.setSensor(this.posRow + 1, this.posCol + 1, this.robotDir);
                SRFrontCenter.setSensor(this.posRow, this.posCol + 1, this.robotDir);
                SRFrontRight.setSensor(this.posRow - 1, this.posCol + 1, this.robotDir);
                SRRightTop.setSensor(this.posRow - 1, this.posCol + 1, findNewDirection(MOVEMENT.RIGHT));
                SRRightBottom.setSensor(this.posRow - 1, this.posCol - 1, findNewDirection(MOVEMENT.RIGHT));
                LRLeft.setSensor(this.posRow + 1, this.posCol + 1, findNewDirection(MOVEMENT.LEFT));
                break;
            case SOUTH:
                SRFrontLeft.setSensor(this.posRow - 1, this.posCol + 1, this.robotDir);
                SRFrontCenter.setSensor(this.posRow - 1, this.posCol, this.robotDir);
                SRFrontRight.setSensor(this.posRow - 1, this.posCol - 1, this.robotDir);
                SRRightTop.setSensor(this.posRow - 1, this.posCol - 1, findNewDirection(MOVEMENT.RIGHT));
                SRRightBottom.setSensor(this.posRow + 1, this.posCol - 1, findNewDirection(MOVEMENT.RIGHT));
                LRLeft.setSensor(this.posRow - 1, this.posCol + 1, findNewDirection(MOVEMENT.LEFT));
                break;
            case WEST:
                SRFrontLeft.setSensor(this.posRow - 1, this.posCol - 1, this.robotDir);
                SRFrontCenter.setSensor(this.posRow, this.posCol - 1, this.robotDir);
                SRFrontRight.setSensor(this.posRow + 1, this.posCol - 1, this.robotDir);
                SRRightTop.setSensor(this.posRow + 1, this.posCol - 1, findNewDirection(MOVEMENT.RIGHT));
                SRRightBottom.setSensor(this.posRow + 1, this.posCol + 1, findNewDirection(MOVEMENT.RIGHT));
                LRLeft.setSensor(this.posRow - 1, this.posCol - 1, findNewDirection(MOVEMENT.LEFT));
                break;
        }

    }

    /**
     * Uses the current direction of the robot and the given movement to find the new direction of the robot.
     */
    private DIRECTION findNewDirection(MOVEMENT m) {
        if (m == MOVEMENT.RIGHT) {
            return DIRECTION.getNext(robotDir);
        } else {
            return DIRECTION.getPrevious(robotDir);
        }
    }

    /**
     * Calls the .sense() method of all the attached sensors and stores the received values in an integer array.
     *
     * @return [SRFrontLeft, SRFrontCenter, SRFrontRight, SRRightTop, SRRightBottom, LRLeft]
     */
    public int[] sense(Map explorationMap, Map realMap) {
        int[] result = new int[6];

        if (!realBot) {
            result[0] = SRFrontLeft.sense(explorationMap, realMap);
            result[1] = SRFrontCenter.sense(explorationMap, realMap);
            result[2] = SRFrontRight.sense(explorationMap, realMap);
            result[3] = SRRightTop.sense(explorationMap, realMap);
            result[4] = SRRightBottom.sense(explorationMap, realMap);
            result[5] = LRLeft.sense(explorationMap, realMap);
        } else {
            CommMgr comm = CommMgr.getCommMgr();
            String msg = comm.recvMsg();

            // String msg = "SDATA;1_2;2_-1;3_-1;4_1;5_-1;6_2;SDATA;1_2;2_1;3_-1;4_1;5_-1;6_3;SDATA;1_2;2_-1;3_-1;4_1;5_-1;6_3;";

            String[] msgArr = msg.split(";");
            int i;
            
            for(i=1;i<7;i++){
            int n = msgArr.length / 7;
            int j;
            HashMap<Integer, Integer> freq = new HashMap<Integer, Integer>();
            int mostFreq = Integer.parseInt(msgArr[i].split("_")[1]);

            for(j=0;j<n;j++){
                int value = Integer.parseInt(msgArr[i+7*j].split("_")[1]);
                if(!freq.containsKey(value)){
                    freq.put(value, 1);
                } else freq.put(value, freq.get(value)+1);
                
                if(freq.get(value) > freq.get(mostFreq)) mostFreq = value;
            }
            result[i-1] = mostFreq;
            }

            // if (msgArr[0].equals(CommMgr.SENSOR_DATA)) {
            //     result[0] = Integer.parseInt(msgArr[1].split("_")[1]);
            //     result[1] = Integer.parseInt(msgArr[2].split("_")[1]);
            //     result[2] = Integer.parseInt(msgArr[3].split("_")[1]);
            //     result[3] = Integer.parseInt(msgArr[4].split("_")[1]);
            //     result[4] = Integer.parseInt(msgArr[5].split("_")[1]);
            //     result[5] = Integer.parseInt(msgArr[6].split("_")[1]);
            // }

            // //Fantom block due to poor center sensor
            // if(result[0] == result[2]) result[1] = result[0];

            System.out.println(Arrays.toString(result));

            SRFrontLeft.senseReal(explorationMap, result[0]);
            SRFrontCenter.senseReal(explorationMap, result[1]);
            SRFrontRight.senseReal(explorationMap, result[2]);
            SRRightTop.senseReal(explorationMap, result[3]);
            SRRightBottom.senseReal(explorationMap, result[4]);
            LRLeft.senseReal(explorationMap, result[5]);

            String[] mapStrings = MapDescriptor.generateMapDescriptor(explorationMap);
            //comm.sendMsg(mapStrings[0] + " " + mapStrings[1], CommMgr.MAP_STRINGS);
            this.explorationMapString = mapStrings[0] + "|" + mapStrings[1] + "|";
        }

        return result;
    }
}