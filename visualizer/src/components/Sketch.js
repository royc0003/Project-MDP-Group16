import React from 'react';
import Sketch from "react-p5";

export default (props) => {
    const arena = props.arena;

    // height of the canvas
    const width = 450;
    // width of the canvas
    const height = 600;

    const pad = 10

    const offsetX = pad;
    const offsetY = pad;
 
    const setup = (p5, canvasParentRef) => {
        // use parent to render the canvas in this ref
        // (without that p5 will render the canvas outside of your component)
        p5.createCanvas(width + 2 * offsetX, height + 2 * offsetY).parent(canvasParentRef);
    };
 
    const backgroundColor = [100, 100, 100];
    const unexploredColor = [100, 100, 100];
    const obstacleColor = [0, 0, 0];
    const exploredColor = [255, 255, 255];
    const robotColor = [0, 0, 0];
    const pathMinColor = [255, 0, 0];
    const pathMaxColor = [0, 0, 255];
    const checkPointColor = [255, 255, 0];

    const drawRect = function(arena, myP5, Xcoord = 0, Ycoord = 0, rColor = [0, 0, 0], size = 0.9) {
        const w = width / arena.Width;
        const h = height / arena.Height;
        const x = Xcoord * w + offsetX;
        const y = Ycoord * h + offsetY;

        const centerX = x + w/2;
        const centerY = y + h/2;

        const widthBox = w * size;
        const heightBox = h * size;
        const topleftX = centerX - widthBox/2;
        const topleftY = centerY - heightBox/2;

        const p5 = myP5;
        if(p5){
            p5.fill(p5.color(...rColor));
            p5.rect(topleftX, topleftY, widthBox, heightBox);
        }
    }

    const drawRobot = function(arena, p5, Xcoord = 0, Ycoord = 0, direction = "", rColor = [0, 0, 0], drawCircle = true) {
        const w = width / arena.Width;
        const h = height / arena.Height;
        const x = Xcoord * w + offsetX;
        const y = Ycoord * h + offsetY;

        if (drawCircle) {
            const centerX = x + w/2;
            const centerY = y + h/2;
            const wR = 3 * w;
            const hR = 3 * h;

            p5.fill(p5.color(...exploredColor));
            p5.ellipse(centerX, centerY, wR, hR);
        }


        const topleft = {x: x, y: y};
        const topright = {x: x + w, y: y};
        const botleft = {x: x, y: y + h};
        const botright = {x: x + w, y: y + h};
        const top = {x: x + w/2, y: y};
        const bot = {x: x + w/2, y: y+h};
        const left = {x: x, y: y + h/2};
        const right = {x: x+w, y: y + h/2};

        p5.fill(p5.color(...rColor));
        switch (direction) {
            case "Up":
                p5.triangle(botleft.x, botleft.y, botright.x, botright.y, top.x, top.y);
                break;
            case "Down":
                p5.triangle(topleft.x, topleft.y, topright.x, topright.y, bot.x, bot.y);
                break;
            case "Right":
                p5.triangle(topleft.x, topleft.y, botleft.x, botleft.y, right.x, right.y);
                break;
            case "Left":
                p5.triangle(topright.x, topright.y, botright.x, botright.y, left.x, left.y);
                break;
        }
    }

    const drawArena = function(arena = {}, p5) {
        p5.background(p5.color(...backgroundColor));
        // arena
        if (arena.Arena != null) {
            for (let Xcoord=0; Xcoord<arena.Width; Xcoord++)
            for (let Ycoord=0; Ycoord<arena.Height; Ycoord++) {
                if (arena.Arena[Ycoord][Xcoord] === "unknown") {
                    drawRect(arena, p5, Xcoord, Ycoord, unexploredColor);
                }
                if (arena.Arena[Ycoord][Xcoord] === "empty") {
                    drawRect(arena, p5, Xcoord, Ycoord, exploredColor);
                }
                if (arena.Arena[Ycoord][Xcoord] === "obstacle") {
                    drawRect(arena, p5, Xcoord, Ycoord, obstacleColor);
                }
            }
        }
        // checkpoint
        const checkPoint = arena.CheckPoint;
        if (checkPoint != null) {
            let {Xcoord, Ycoord} = checkPoint;
            Ycoord = arena.Height - 1 - Ycoord;
            drawRect(p5, arena, Xcoord, Ycoord, checkPointColor, 0.9);
        }
        // robot
        if (arena.Current != null) {
            const Xcoord = arena.Current.Xcoord;
            const Ycoord = arena.Height - 1 - arena.Current.Ycoord;
            drawRobot(p5, arena, Xcoord, Ycoord, arena.Current.Direction, robotColor);
        }
    };

    let update = true;
    const draw = (p5) => {
        if (update) {
            drawArena(arena, p5);
        }
    };
 
    return <Sketch setup={setup} draw={draw} />;
};