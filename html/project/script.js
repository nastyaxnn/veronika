let red = 255;
let green = 0;
let blue = 0;

let bgValue = "rgb(" + red + ", " + green + ", " + blue + ")";
console.log(bgValue);


red = prompt('Введите насыщенность красного в виде числа от 0 до 255', 255);
green = prompt('Введите насыщенность зелёного в виде числа от 0 до 255', 0);
blue = prompt('Введите насыщенность синего в виде числа от 0 до 255', 0);

console.log(red + green + blue); 
red = Number(red);
blue = Number(blue);
green = +green;
console.log(red + green + blue); 

if (isNaN(red)) {
    red = Number(prompt('В качестве значения насыщенности красного вы ввели не число. Пожалуйста, введите число от 0 до 255.', 255));
    console.log('Вы определили насыщенность красного как ' + red);
} else if (red < 0) {
    red = 0;
    console.log('Наименьшее из возможных чисел — ноль, мы подставили значение 0 для красного.');
} else if (red > 255) {
    red = 255;
    console.log('Наибольшее возможное число — 255, мы подставили его для красного.');
} else {
    console.log('Вы определили насыщенность красного как ' + red);
}

if (isNaN(green)) {
    green = Number(prompt('В качестве значения насыщенности зеленого вы ввели не число. Пожалуйста, введите число от 0 до 255.', 255));
    console.log('Вы определили насыщенность зеленого как ' + green);
} else if (green < 0) {
    green = 0;
    console.log('Наименьшее из возможных чисел — ноль, мы подставили значение 0 для зеленого.');
} else if (red > 255) {
    green = 255;
    console.log('Наибольшее возможное число — 255, мы подставили его для зеленого.');
} else {
    console.log('Вы определили насыщенность зеленого как ' + green);
}

if (isNaN(blue)) {
    blue = Number(prompt('В качестве значения насыщенности синего вы ввели не число. Пожалуйста, введите число от 0 до 255.', 255));
    console.log('Вы определили насыщенность синего как ' + blue);
} else if (blue < 0) {
    blue = 0;
    console.log('Наименьшее из возможных чисел — ноль, мы подставили значение 0 для синего.');
} else if (blue > 255) {
    blue = 255;
    console.log('Наибольшее возможное число — 255, мы подставили его для синего.');
} else {
    console.log('Вы определили насыщенность синего как ' + blue);
}

function makeColorString(r, g, b) {
    return "rgb(" + r + ", " + g + ", " + b + ")";
}

function checkInput(i) {
    i = +i;
    if (isNaN(i)) {
        i = Number(prompt('Введите корректное число от 0 до 255', 255));
    } else if (i < 0) {
        i = 0;
        console.log('Наименьшее из возможных чисел — ноль, мы подставили значение 0.');
    } else if (i > 255) {
        i = 255;
        console.log('Наибольшее возможное число — 255, мы подставили его.');
    }
    return i;
}

red = checkInput(red);
green = checkInput(green);
blue = checkInput(blue);

document.write(makeColorString(red, green, blue));

document.body.style.backgroundColor = makeColorString(red, green, blue);
document.body.style.fontSize = "100px";
document.body.style.fontFamily = "Anonymous Pro";
document.body.style.color = "white";

let pageSettings = {
    red: 200,
    green: 200,
    blue: 200
};
let bgColor = "rgb(" + pageSettings.red + "," + pageSettings.green + "," + pageSettings.blue + ")";
document.body.style.backgroundColor = bgColor;


let header = document.getElementById('main-header');
console.log(header);
pageSettings.background = 'https://pictures.s3.yandex.net/cover-wall.jpg';
header.style.backgroundImage = 'url(' + pageSettings.background + ')';

pageSettings.background = [
    'https://pictures.s3.yandex.net/background.jpg',
    'https://pictures.s3.yandex.net/cover-color.jpg',
    'https://pictures.s3.yandex.net/cover-grid.jpg',
    'https://pictures.s3.yandex.net/cover-typo.jpg',
    'https://pictures.s3.yandex.net/cover-wall.jpg'
];
header.style.backgroundImage = 'url(' + pageSettings.background[2] + ')';
header.style.backgroundImage = 'url(' + pageSettings.background[pageSettings.background.length - 1] + ')';


let cards = document.getElementsByClassName('card');
cards[1].style.color = 'navy';

window.addEventListener('scroll', function() {
    let scrollY = window.pageYOffset;
    let bgValue = 'rgb(' + 
    (255 - scrollY/3) + ', ' + 
    (255 - scrollY/3) + ', ' + 
    (255 - scrollY/3) + ')';

    document.body.style.backgroundColor = bgValue;

    let cards = document.getElementsByClassName('card');
    for (let c = 0; c < cards.length; c++) {
        let card = cards[c];
        card.style.color = 'rgb(' + (255 - scrollY/2) + ', 0, ' + (scrollY/4) + ')';
    }
});