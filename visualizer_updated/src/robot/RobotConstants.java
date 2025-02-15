package robot;

/**
 * Constants used in this package.
 *
 */

public class RobotConstants {
    public static final int GOAL_ROW = 18;                          // row no. of goal cell
    public static final int GOAL_COL = 13;                          // col no. of goal cell
    public static final int START_ROW = 1;                          // row no. of start cell
    public static final int START_COL = 1;                          // col no. of start cell
    public static final int MOVE_COST = 10;                         // cost of FORWARD, BACKWARD movement
    public static final int TURN_COST = 20;                         // cost of RIGHT, LEFT movement
    public static final int SPEED = 200; //400; 978                 // delay between movements (ms)
    public static final DIRECTION START_DIR = DIRECTION.NORTH;      // start direction
    public static final int SENSOR_SHORT_RANGE_L = 1;               // range of short range sensor (cells)
    public static final int SENSOR_SHORT_RANGE_H = 2;               // range of short range sensor (cells)
    public static final int SENSOR_LONG_RANGE_L = 1;                // range of long range sensor (cells)
    public static final int SENSOR_LONG_RANGE_H = 4;                // range of long range sensor (cells)

    public static final int INFINITE_COST = 9999;
    //some Notes:
    // note that oridnal returns the index of the given array
    // also note that values return all the values of the current enum
    // Assume current direction is north index = 0 --> (0+1) % 4 == 1
    // For previous direction assuming 1; (1 + 3) % 4 == 0
    public enum DIRECTION {
        NORTH, EAST, SOUTH, WEST;

        public static DIRECTION getNext(DIRECTION curDirection) {
            return values()[(curDirection.ordinal() + 1) % values().length];
        }

        public static DIRECTION getPrevious(DIRECTION curDirection) {
            return values()[(curDirection.ordinal() + values().length - 1) % values().length];
        }

        public static char print(DIRECTION d) {
            switch (d) {
                case NORTH:
                    return 'N';
                case EAST:
                    return 'E';
                case SOUTH:
                    return 'S';
                case WEST:
                    return 'W';
                default:
                    return 'X';
            }
        }
    }

    public enum MOVEMENT {
        FORWARD, BACKWARD, RIGHT, LEFT,
        CALIBRATE_RIGHT, CALIBRATE_DISTANCE, CALIBRATE_ANGLE_LR, CALIBRATE_ANGLE_LC, CALIBRATE_ANGLE_RC, CALIBRATE_FRONT_RIGHT, CALIBRATE_FRONT_LEFT,
        ERROR,
        SPECIAL_RIGHT, SPECIAL_LEFT;

        public static char print(MOVEMENT m) {
            switch (m) {
                case FORWARD:
                    return 'F';
                case BACKWARD:
                    return 'B';
                case RIGHT:
                    return 'R';
                case LEFT:
                    return 'L';
                case CALIBRATE_RIGHT:
                    return 'Y';
                case CALIBRATE_DISTANCE:
                    return 'X';
                case CALIBRATE_ANGLE_LR:
                    return 'O';
                case CALIBRATE_ANGLE_LC:
                    return 'P';
                case CALIBRATE_ANGLE_RC:
                    return 'Q';
                case SPECIAL_RIGHT:
                    return 'D';
                case SPECIAL_LEFT:
                    return 'A';
                case CALIBRATE_FRONT_LEFT:
                    return 'V';
                case CALIBRATE_FRONT_RIGHT:
                    return 'N';
                case ERROR:
                default:
                    return 'E';
            }
        }
    }
}