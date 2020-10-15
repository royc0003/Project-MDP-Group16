package robot;

import map.Map;
import robot.RobotConstants.DIRECTION;

/**
 * Represents a sensor mounted on the robot.
 *
 */

public class Sensor {
    private final int lowerRange;
    private final int upperRange;
    private int sensorPosRow;
    private int sensorPosCol;
    private DIRECTION sensorDir;
    private final String id;
    private int sensorVal;

    public Sensor(int lowerRange, int upperRange, int row, int col, DIRECTION dir, String id) {
        this.lowerRange = lowerRange;
        this.upperRange = upperRange;
        this.sensorPosRow = row;
        this.sensorPosCol = col;
        this.sensorDir = dir;
        this.id = id;
    }

    public void setSensor(int row, int col, DIRECTION dir) {
        this.sensorPosRow = row;
        this.sensorPosCol = col;
        this.sensorDir = dir;
    }

    public int getSensorVal(){
        return this.sensorVal;
    }
    /**
     * Returns the number of cells to the nearest detected obstacle or -1 if no obstacle is detected.
     */
    public int sense(Map exploredMap, Map realMap) {
        switch (sensorDir) {
            case NORTH:
                return getSensorVal(exploredMap, realMap, 1, 0);
            case EAST:
                return getSensorVal(exploredMap, realMap, 0, 1);
            case SOUTH:
                return getSensorVal(exploredMap, realMap, -1, 0);
            case WEST:
                return getSensorVal(exploredMap, realMap, 0, -1);
        }
        return -1;
    }

    /**
     * Sets the appropriate obstacle cell in the map and returns the row or column value of the obstacle cell. Returns
     * -1 if no obstacle is detected.
     */
    private int getSensorVal(Map exploredMap, Map realMap, int rowInc, int colInc) {
        // Check if starting point is valid for sensors with lowerRange > 1.
        //
        if (lowerRange > 1) {
            for (int i = 1; i < this.lowerRange; i++) {
                int row = this.sensorPosRow + (rowInc * i);
                int col = this.sensorPosCol + (colInc * i);
                //if not a valid coordinate return i
                if (!exploredMap.checkValidCoordinates(row, col)) return i;
                if (realMap.getCell(row, col).getIsObstacle()) return i;
            }
        }

        // Check if anything is detected by the sensor and return that value.
        for (int i = this.lowerRange; i <= this.upperRange; i++) {
            int row = this.sensorPosRow + (rowInc * i);
            int col = this.sensorPosCol + (colInc * i);

            if (!exploredMap.checkValidCoordinates(row, col)) return i;

            exploredMap.getCell(row, col).setIsExplored(true);

            if (realMap.getCell(row, col).getIsObstacle()) {
                exploredMap.setObstacleCell(row, col, true);
                return i;
            }
        }

        // Else, return -1.
        return -1;
    }

    /**
     * Uses the sensor direction and given value from the actual sensor to update the map.
     */
    public void senseReal(Map exploredMap, int sensorVal) {
        this.sensorVal = sensorVal;
        switch (sensorDir) {
            case NORTH:
                processSensorVal(exploredMap, sensorVal, 1, 0);
                break;
            case EAST:
                processSensorVal(exploredMap, sensorVal, 0, 1);
                break;
            case SOUTH:
                processSensorVal(exploredMap, sensorVal, -1, 0);
                break;
            case WEST:
                processSensorVal(exploredMap, sensorVal, 0, -1);
                break;
        }
    }

    /**
     * Sets the correct cells to explored and/or obstacle according to the actual sensor value.
     */
    private void processSensorVal(Map exploredMap, int sensorVal, int rowInc, int colInc) {
        if (sensorVal == 0) return;  // return value for LR sensor if obstacle before lowerRange

         //If above fails, check if starting point is valid for sensors with lowerRange > 1.
//         for (int i = 1; i < this.lowerRange; i++) {
//             int row = this.sensorPosRow + (rowInc * i);
//             int col = this.sensorPosCol + (colInc * i);
//
//             if (!exploredMap.checkValidCoordinates(row, col)) return;
//             if (exploredMap.getCell(row, col).getIsObstacle()) return;
//         }

        // Update map according to sensor's value.
        for (int i = this.lowerRange; i <= this.upperRange; i++) {
            int row = this.sensorPosRow + (rowInc * i);
            int col = this.sensorPosCol + (colInc * i);

            if (!exploredMap.checkValidCoordinates(row, col)) continue;
            // if is explored, don't set obstacle
            if (!id.equals("SRFL") && !id.equals("SRFC") && !id.equals("SRFR") && exploredMap.getCell(row, col).getIsExplored()) continue;

            if(sensorVal == -1 ){ //handle sensor value if -1; to prevent from going beyond set obstacle; don't explore beyond
                int prevRow = this.sensorPosRow + (rowInc * (i-1));
                int prevCol = this.sensorPosCol + (colInc * (i-1));
                if (exploredMap.getCell(prevRow, prevCol).getIsObstacle()) return;
            }
            exploredMap.getCell(row, col).setIsExplored(true);

            if (sensorVal == i) {
                // if((id.equals("SRRB") || id.equals("LRL")) && exploredMap.getCell(row, col).getIsExplored()){
                //     return;
                // }
                exploredMap.setObstacleCell(row, col, true);
                break;
            }

            // Override previous obstacle value if front sensors detect no obstacle.
            if (exploredMap.getCell(row, col).getIsObstacle()) {
                if (id.equals("SRFL") || id.equals("SRFC") || id.equals("SRFR")) {
                    System.out.println("Overwriting: [row, col]: "+row+" "+col);
                    exploredMap.setObstacleCell(row, col, false);
                } else {
                    break;
                }
            }
        }
    }
}