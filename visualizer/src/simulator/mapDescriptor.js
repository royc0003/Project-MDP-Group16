const toBit = {
    "0": [0,0,0,0],
    "1": [0,0,0,1],
    "2": [0,0,1,0],
    "3": [0,0,1,1],
    "4": [0,1,0,0],
    "5": [0,1,0,1],
    "6": [0,1,1,0],
    "7": [0,1,1,1],
    "8": [1,0,0,0],
    "9": [1,0,0,1],
    "A": [1,0,1,0],
    "B": [1,0,1,1],
    "C": [1,1,0,0],
    "D": [1,1,0,1],
    "E": [1,1,1,0],
    "F": [1,1,1,1],
    };
    
// export const showMapDescriptor = (arenaMode) => {
//     drawArena({
//     Height: 20,
//     Width: 15,
//     Arena: getMapDescriptor(arenaMode.toUpperCase())
//     });
// }
    
export const getMapDescriptor = (value) => {
    let bitArray = [];
    for (let i=0; i<value.length; i++) {
    const arr = toBit[value[i]];
    if (arr != undefined)
        bitArray = [...bitArray, ...arr];
    }
    bitArray = bitArray.slice(2, 302);

    const width = 15;
    const height = 20;
    let arena = [];
    for (let i=0; i<height; i++) {
    let row = [];
    for (let j=0; j<width; j++)
        row.push("empty");
    arena.push(row);
    }

    for (let i=0; i<bitArray.length; i++) {
    const x = i % width;
    const y = (height - 1) - Math.floor(i / width);
    if (bitArray[i] == 1) {
        arena[y][x] = "obstacle";
    } else {
        arena[y][x] = "empty";
    }
    }

    return arena;
}