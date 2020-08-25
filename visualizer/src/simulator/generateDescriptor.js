export function generateDescriptor(arenaMode) {
    const widthX = 15
    const widthY = 20
    let bitArray = [1, 1]
    var i, j;
    for (j = widthY - 1; j >= 0; j--)
        for (i = 0; i < widthX; i++)
            if (arenaMode[j][i] === "obstacle")
                bitArray.push(1)
            else bitArray.push(0)

    bitArray.push(1)
    bitArray.push(1)
    return bin2hex(bitArray.join(""));
}

function bin2hex(b) {
    return b.match(/.{4}/g).reduce(function (acc, i) {
        return acc + parseInt(i, 2).toString(16);
    }, '').toUpperCase()
}