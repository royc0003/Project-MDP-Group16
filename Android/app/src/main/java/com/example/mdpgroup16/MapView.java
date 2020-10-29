package com.example.mdpgroup16;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.Typeface;
import android.graphics.drawable.Drawable;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;

import androidx.annotation.Nullable;

public class MapView extends View {

    private static final String TAG = "MazeView";
    private static String OBSTACLES_POS = "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
    private static String EXPLORED_POS = "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
    private static Cell[][] cells;
    private static final int COLS = 15, ROWS = 20;
    private static final float WALL_THICKNESS = 2;
    private static final int START_X = 1, START_Y = 18;
    private static final int GOAL_X = 13, GOAL_Y = 1;
    private static float cellSize, hMargin, vMargin;
    private static int robotRow = 18, robotCol = 1;
    private static int waypointX, waypointY;
    private static String robotDirection = "Down";
    private static boolean loadNumberID = false;
    //Assume that the numberID loaded on the map will not be more than 15
    private static String[][] mapNumberIDString = new String[128][4];
    private static int numberIDCounter = 0;
    public static Canvas mapCanvas;

    private Paint wallPaint, robotPaint, directionPaint, startPaint, goalPaint, gridNumberPaint, waypointPaint, obstaclePaint, exploredPaint, unexploredPaint, numberIDPaint;

    private class Cell {
        float startX, startY, endX, endY;
        Paint paint;

        private Cell(float startX, float startY, float endX, float endY, Paint paint) {
            this.startX = startX;
            this.startY = startY;
            this.endX = endX;
            this.endY = endY;
            this.paint = paint;
        }
    }

    public MapView(Context context) {
        this(context, null);
    }

    public MapView(Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);

        wallPaint = new Paint();
        wallPaint.setColor(Color.parseColor("#64B5F6"));
        wallPaint.setStrokeWidth(WALL_THICKNESS);

        robotPaint = new Paint();
        robotPaint.setColor(Color.parseColor("#e84545"));

        directionPaint = new Paint();
        directionPaint.setColor(Color.parseColor("#53354a"));

        unexploredPaint = new Paint();
        unexploredPaint.setColor(Color.parseColor("#F5F5F5"));

        exploredPaint = new Paint();
        exploredPaint.setColor(Color.parseColor("#ffb6b9"));

        waypointPaint = new Paint();
        waypointPaint.setColor(Color.parseColor("#08d9d6"));

        gridNumberPaint = new Paint();
        gridNumberPaint.setColor(Color.WHITE);
        gridNumberPaint.setTextSize(18);
        gridNumberPaint.setTypeface(Typeface.DEFAULT);

        startPaint = new Paint();
        startPaint.setColor(Color.parseColor("#fce38a"));

        goalPaint = new Paint();
        goalPaint.setColor(Color.parseColor("#ffc93c"));

        obstaclePaint = new Paint();
        obstaclePaint.setColor(Color.parseColor("#8C8C8C"));

        numberIDPaint = new Paint();
        numberIDPaint.setColor(Color.parseColor("#8C8C8C"));
    }

    @Override
    protected void onDraw(Canvas canvas) {
        mapCanvas = canvas;
        alignMap(mapCanvas);
        drawBorder(mapCanvas);
        initCells();
        drawCell(mapCanvas);
        drawGridNumber(mapCanvas);
        drawStartEnd(mapCanvas);
        initExploredObstacles(mapCanvas, EXPLORED_POS, OBSTACLES_POS);
        drawNumberID(mapCanvas);
        initWaypoint(mapCanvas);
        initRobot(mapCanvas);
        drawRobotDirection(mapCanvas);

    }

    private void alignMap(Canvas canvas) {
        //Canvas Color
        //canvas.drawColor(Color.WHITE);

        //Canvas Width & Height
        int width = getWidth();
        int height = getHeight();

        //Margin is at least half of the cell size
        //Two scenarios, add + 1 (Margin on both sides) either to COLS or ROWS
        if (width / height < COLS / ROWS) {
            cellSize = width / (COLS + 1);
        } else {
            cellSize = height / (ROWS + 1);
        }

        //Get margin on each side for horizontal and vertical
        hMargin = (width - COLS * cellSize) / 2;
        vMargin = ((height - ROWS * cellSize) / 2);

        //set margin in place
        canvas.translate(hMargin, vMargin);
    }

    //Helper function to draw the cells for Start/End & Robot
    private void drawGrid(Canvas canvas, int x, int y, Paint gridPaint) {
        canvas.drawRect(cells[x][y].startX, cells[x][y].startY, cells[x][y].endX, cells[x][y].endY, gridPaint);
        //top
        canvas.drawRect(cells[x][y - 1].startX, cells[x][y - 1].startY, cells[x][y - 1].endX, cells[x][y - 1].endY, gridPaint);
        //bottom
        canvas.drawRect(cells[x][y + 1].startX, cells[x][y + 1].startY, cells[x][y + 1].endX, cells[x][y + 1].endY, gridPaint);
        //left
        canvas.drawRect(cells[x - 1][y].startX, cells[x - 1][y].startY, cells[x - 1][y].endX, cells[x - 1][y].endY, gridPaint);
        //right
        canvas.drawRect(cells[x + 1][y].startX, cells[x + 1][y].startY, cells[x + 1][y].endX, cells[x + 1][y].endY, gridPaint);
        //diagonal top left
        canvas.drawRect(cells[x - 1][y - 1].startX, cells[x - 1][y - 1].startY, cells[x - 1][y - 1].endX, cells[x - 1][y - 1].endY, gridPaint);
        //diagonal top right
        canvas.drawRect(cells[x + 1][y - 1].startX, cells[x + 1][y - 1].startY, cells[x + 1][y - 1].endX, cells[x + 1][y - 1].endY, gridPaint);
        //diagonal bottom left
        canvas.drawRect(cells[x - 1][y + 1].startX, cells[x - 1][y + 1].startY, cells[x - 1][y + 1].endX, cells[x - 1][y + 1].endY, gridPaint);
        //diagonal bottom right
        canvas.drawRect(cells[x + 1][y + 1].startX, cells[x + 1][y + 1].startY, cells[x + 1][y + 1].endX, cells[x + 1][y + 1].endY, gridPaint);
    }

    //Draw out the 15x20 grid
    private void drawBorder(Canvas canvas) {
        for (int x = 0; x < COLS; x++) {
            for (int y = 0; y < ROWS; y++) {
                //Draw line for top
                canvas.drawLine(
                        x * cellSize,
                        y * cellSize,
                        (x + 1) * cellSize,
                        y * cellSize, wallPaint
                );
                //Draw line for left
                canvas.drawLine(
                        x * cellSize,
                        y * cellSize,
                        x * cellSize,
                        (y + 1) * cellSize, wallPaint
                );
                //Draw line for bottom
                canvas.drawLine(
                        x * cellSize,
                        (y + 1) * cellSize,
                        (x + 1) * cellSize,
                        (y + 1) * cellSize, wallPaint
                );
                //Draw line for right
                canvas.drawLine(
                        (x + 1) * cellSize,
                        y * cellSize,
                        (x + 1) * cellSize,
                        (y + 1) * cellSize, wallPaint
                );
            }
        }
    }

    //Create a new cell with margin in each grid box
    private void initCells() {
        cells = new Cell[COLS][ROWS];

        for (int x = 0; x < COLS; x++) {
            for (int y = 0; y < ROWS; y++) {
                cells[x][y] = new Cell(x * cellSize + (cellSize / 30),
                        y * cellSize + (cellSize / 30),
                        (x + 1) * cellSize - (cellSize / 40),
                        (y + 1) * cellSize - (cellSize / 60), unexploredPaint);
            }
        }

    }

    //Draw each cell in the border box
    private void drawCell(Canvas canvas) {

        for (int x = 0; x < COLS; x++) {
            for (int y = 0; y < ROWS; y++) {
                canvas.drawRect(cells[x][y].startX, cells[x][y].startY, cells[x][y].endX, cells[x][y].endY, cells[x][y].paint);
                //Log.d(TAG, "Start X: " + cells[x][y].startX + " Start Y: " + cells[x][y].startY + " End X: " + cells[x][y].endX + " End Y: " + cells[x][y].endY);
            }
        }
    }

    //Draw grid number
    private void drawGridNumber(Canvas canvas) {

        for (int x = 0; x < 15; x++) {

            if (x > 9 && x < 15) {

                canvas.drawText(Integer.toString(x), cells[x][19].startX + (cellSize / 5), cells[x][19].endY + (cellSize / 1.5f), gridNumberPaint);
            } else {
                //GRID NUMBER FOR ROW
                canvas.drawText(Integer.toString(x), cells[x][19].startX + (cellSize / 3), cells[x][19].endY + (cellSize / 1.5f), gridNumberPaint);

            }
        }

        for (int x = 0; x < 20; x++) {

            if (x > 9 && x < 20) {

                canvas.drawText(Integer.toString(19 - x), cells[0][x].startX - (cellSize / 1.5f), cells[0][x].endY - (cellSize / 3.5f), gridNumberPaint);
            } else {

                canvas.drawText(Integer.toString(19 - x), cells[0][x].startX - (cellSize / 1.2f), cells[0][x].endY - (cellSize / 3.5f), gridNumberPaint);

            }
        }
    }

    private void drawStartEnd(Canvas canvas) {
        drawGrid(canvas, START_X, START_Y, startPaint);
        drawGrid(canvas, GOAL_X, GOAL_Y, goalPaint);
    }

    private void initRobot(Canvas canvas) {
        drawGrid(canvas, robotCol, robotRow, robotPaint);
    }

    private void initWaypoint(Canvas canvas) {
        //Don't draw waypoint block if it is default
        if (!(waypointX == 0 && waypointY == 0)) {
            canvas.drawRect(cells[waypointX][waypointY].startX, cells[waypointX][waypointY].startY, cells[waypointX][waypointY].endX, cells[waypointX][waypointY].endY, waypointPaint);
        }
    }

    private Drawable getImageResource(int numberID) {
        Drawable imageResource;
        switch (numberID) {
            case 1:
                imageResource = getResources().getDrawable(R.drawable.up_arrow, null);
                return imageResource;
            case 2:
                imageResource = getResources().getDrawable(R.drawable.down_arrow, null);
                return imageResource;
            case 3:
                imageResource = getResources().getDrawable(R.drawable.right_arrow, null);
                return imageResource;
            case 4:
                imageResource = getResources().getDrawable(R.drawable.left_arrow, null);
                return imageResource;
            case 5:
                imageResource = getResources().getDrawable(R.drawable.stop, null);
                return imageResource;
            case 6:
                imageResource = getResources().getDrawable(R.drawable.six, null);
                return imageResource;
            case 7:
                imageResource = getResources().getDrawable(R.drawable.seven, null);
                return imageResource;
            case 8:
                imageResource = getResources().getDrawable(R.drawable.eight, null);
                return imageResource;
            case 9:
                imageResource = getResources().getDrawable(R.drawable.nine, null);
                return imageResource;
            case 10:
                imageResource = getResources().getDrawable(R.drawable.zero, null);
                return imageResource;
            case 11:
                imageResource = getResources().getDrawable(R.drawable.letter_v, null);
                return imageResource;
            case 12:
                imageResource = getResources().getDrawable(R.drawable.letter_w, null);
                return imageResource;
            case 13:
                imageResource = getResources().getDrawable(R.drawable.letter_x, null);
                return imageResource;
            case 14:
                imageResource = getResources().getDrawable(R.drawable.letter_y, null);
                return imageResource;
            case 15:
                imageResource = getResources().getDrawable(R.drawable.letter_z, null);
                return imageResource;
            //End of ABCDE
            default:
                //Return random image
                imageResource = getResources().getDrawable(R.drawable.reload, null);
                return imageResource;
        }
    }

    private void drawRobotDirection(Canvas canvas) {
        switch (robotDirection) {
            case "D":
                canvas.drawRect(cells[robotCol][robotRow + 1].startX,
                        cells[robotCol][robotRow + 1].startY,
                        cells[robotCol][robotRow + 1].endX,
                        cells[robotCol][robotRow + 1].endY, directionPaint);

                break;
            case "L":
                canvas.drawRect(cells[robotCol - 1][robotRow].startX,
                        cells[robotCol - 1][robotRow].startY,
                        cells[robotCol - 1][robotRow].endX,
                        cells[robotCol - 1][robotRow].endY,
                        directionPaint);
                break;
            case "R":
                canvas.drawRect(cells[robotCol + 1][robotRow].startX,
                        cells[robotCol + 1][robotRow].startY,
                        cells[robotCol + 1][robotRow].endX,
                        cells[robotCol + 1][robotRow].endY,
                        directionPaint);
                break;
            default:
                canvas.drawRect(cells[robotCol][robotRow - 1].startX,
                        cells[robotCol][robotRow - 1].startY,
                        cells[robotCol][robotRow - 1].endX,
                        cells[robotCol][robotRow - 1].endY,
                        directionPaint);
        }
    }

    public void setRobotCoordinates(int col, int row) {
        //Check if the col and row is within the grid
        Log.d(TAG, "Col " + col + "Row " + row);
        if ((col >= 0 && col <= 14) && (row >= 0 && row <= 19)) {
            robotCol = col;
            robotRow = Math.abs(row - 19);
            //Recycle view
            invalidate();
        }
    }

    public void setRobotDirection(String direction) {
        robotDirection = direction;
        //Recycle view
        invalidate();
    }

    public void checkwp(int x, int y) {
        waypointX = x;
        waypointY = y;
        //Recycle view
        invalidate();
    }


    public int[] setWaypointOrRobot(float x, float y) {
        int coordinates[];
        int isWaypoint = 0;
        //Log.d(TAG, "X " + x + " " + "y " + y);
        coordinates = getSelectedRowCol(x, y);
        int selectedCol = coordinates[0];
        int selectedRow = coordinates[1];
        //Log.d(TAG, "selectedCol " + selectedCol + " " + "selectedRow " + selectedRow);
        if (selectedRow >= 0 && selectedCol >= 0) {
            //Check to see if the selected box is not at the first or last row/cols
            if ((selectedCol != 0 && selectedCol != 14) && (selectedRow != 0 && selectedRow != 19)) {
                if (MainActivity.wayPointChecked) {
                    waypointX = selectedCol;
                    waypointY = selectedRow;
                    isWaypoint = 1;
                }
                //Disable cos no need to set robot position
                //To enable it, go to map tab also
                //else {
                //    robotCol = selectedCol;
                //    robotRow = selectedRow;
                //}
            }
        }
        //Recycle view
        invalidate();
        return new int[]{selectedCol, selectedRow, isWaypoint};
    }

    public int[] getSelectedRowCol(float x, float y) {
        int row = -1, cols = -1;

        //Get selected column index
        for (int i = 0; i < COLS; i++) {
            if (cells[i][0].endX >= (x - hMargin) && cells[i][0].startX <= (x - hMargin)) {
                cols = i;
                break;
            }
        }

        int counter = 0;
        //Get selected row index
        for (int j = ROWS - 1; j >= 0; j--) {
            if (cells[0][j].endY >= (y - vMargin) && cells[0][j].startY <= (y - vMargin)) {
                row = ROWS - counter - 1;
                break;
            }
            counter++;
        }
        return new int[]{cols, row};
    }

    //Conversion from Hexadecimal to Decimal to Binary
    public void setMapExploredObstacles(String exploredHex, String obstaclesHex) {
        // 5 Hex digit each time to prevent overflow
        String mdfStringBin = "";
        String mdfStringBin1 = "";
        String bin = "";
        String bin1 = "";
        String partial;
        String partial1;
        int pointer = 0;
        int pointer1 = 0;
        //Obstacles Part
        while (obstaclesHex.length() - pointer > 0) {
            partial = obstaclesHex.substring(pointer, pointer + 1);
            bin = Integer.toBinaryString(Integer.parseInt(partial, 16));
            for (int i = 0; i < 4 - bin.length(); i++) {
                mdfStringBin = mdfStringBin.concat("0");
            }
            mdfStringBin = mdfStringBin.concat(bin);
            pointer += 1;
        }
        //Explored Part
        while (exploredHex.length() - pointer1 > 0) {
            partial1 = exploredHex.substring(pointer1, pointer1 + 1);
            bin1 = Integer.toBinaryString(Integer.parseInt(partial1, 16));
            for (int i = 0; i < 4 - bin1.length(); i++) {
                mdfStringBin1 = mdfStringBin1.concat("0");
            }
            mdfStringBin1 = mdfStringBin1.concat(bin1);
            pointer1 += 1;
        }

        OBSTACLES_POS = mdfStringBin;
        EXPLORED_POS = mdfStringBin1;
        // RecycleView
        invalidate();
    }

    public void initExploredObstacles(Canvas canvas, String exploredBinaryInfo, String obstacleBinaryInfo) {
        //int binaryStringLength = obstacleBinaryInfo.length();
        //Log.d(TAG, "Length:" + binaryStringLength);

        //Cut the 11 padding top and bottom
        exploredBinaryInfo = exploredBinaryInfo.substring(2, 302);
        int obstacleBinary;
        int exploredBinary;
        //int counter = 0;
        for (int j = ROWS - 1; j >= 0; j--) {
            for (int k = 0; k < COLS; k++) {
                exploredBinary = (int) exploredBinaryInfo.charAt(0);
                exploredBinaryInfo = exploredBinaryInfo.substring(1);
                if (exploredBinary == 49) {
                    //Log.d(TAG, "Check: " + alphabet);
                    canvas.drawRect(cells[k][j].startX, cells[k][j].startY, cells[k][j].endX, cells[k][j].endY, exploredPaint);


                    obstacleBinary = (int) obstacleBinaryInfo.charAt(0);
                    obstacleBinaryInfo = obstacleBinaryInfo.substring(1);
                    //ASCII for 1 is 49
                    if (obstacleBinary == 49) {
                        //Log.d(TAG, "Check: " + alphabet);
                        canvas.drawRect(cells[k][j].startX, cells[k][j].startY, cells[k][j].endX, cells[k][j].endY, obstaclePaint);
                    }}
            }
        }
    }

    public void drawNumberID(Canvas canvas) {
        if (loadNumberID) {
            for (int i = 0; i < numberIDCounter; i++) {
                int x = Integer.parseInt(mapNumberIDString[i][0]);
                int y = Math.abs(Integer.parseInt(mapNumberIDString[i][1]) - 19);
                int numberID = Integer.parseInt(mapNumberIDString[i][2]);
                String direction = mapNumberIDString[i][3];
                Drawable directionResouce;

                int arrowPositionCol = x;
                int arrowPositionRow = y;

                Drawable arrows = getImageResource(numberID);
                arrows.setBounds(new Rect((int) (cells[arrowPositionCol][arrowPositionRow].startX), (int) (cells[arrowPositionCol][arrowPositionRow].startY), ((int) cells[arrowPositionCol][arrowPositionRow].endX), ((int) cells[arrowPositionCol][arrowPositionRow].endY)));
                arrows.draw(mapCanvas);

                if (direction.equals("U")) {
                    arrowPositionRow = arrowPositionRow - 1;
                    directionResouce = getResources().getDrawable(R.drawable.downbtn, null);
                } else if (direction.equals("D")) {
                    arrowPositionRow = arrowPositionRow + 1;
                    directionResouce = getResources().getDrawable(R.drawable.upbtn, null);
                } else if (direction.equals("L")) {
                    arrowPositionCol = arrowPositionCol - 1;
                    directionResouce = getResources().getDrawable(R.drawable.rightbtn, null);
                } else {
                    arrowPositionCol = arrowPositionCol + 1;
                    directionResouce = getResources().getDrawable(R.drawable.leftbtn, null);
                }

                directionResouce.setBounds(new Rect((int) (cells[arrowPositionCol][arrowPositionRow].startX), (int) (cells[arrowPositionCol][arrowPositionRow].startY), ((int) cells[arrowPositionCol][arrowPositionRow].endX), ((int) cells[arrowPositionCol][arrowPositionRow].endY)));
                directionResouce.draw(mapCanvas);
            }
        }
    }

    public void initNumberID(String[] numberIDString) {
        for (int i = 0; i < numberIDString.length; i++) {
            //Log.d(TAG, numberIDString[i]);
            mapNumberIDString[numberIDCounter][i] = numberIDString[i];
        }
        numberIDCounter++;
        loadNumberID = true;
        invalidate();
    }

    public void resetMap() {
        //Clear Obstacles and Explored
        OBSTACLES_POS = "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
        EXPLORED_POS = "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
        //Reset robot
        robotRow = 18;
        robotCol = 1;
        robotDirection = "Top";
        //Clear waypoint
        waypointX = 0;
        waypointY = 0;
        //Clear Map NumberID
        loadNumberID = false;
        mapNumberIDString = new String[128][4];
        numberIDCounter = 0;
        //Recycle View
        invalidate();
    }
}
