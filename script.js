// УПРАВЛЕНИЕ КОЛЕСАМИ И СООТВЕТСТВУЮЩАЯ РЕАКЦИЯ
// Необходимые переменные
var mot = 0;
var s_mot = 1;

var rot = 0;
var x4 = 2;//querySelector
var pc4x2c = document.getElementById('pc4x2c');
var pc4x2r = document.getElementById('pc4x2r');
var pc4x2l = document.getElementById('pc4x2l');
var pc4x4c = document.getElementById('pc4x4c');
var pc4x4r = document.getElementById('pc4x4r');
var pc4x4l = document.getElementById('pc4x4l');

var mot_get = document.getElementById('motor_get_real');
var mot_set = document.getElementById('motor_set_real');

// Отображение соответсвующей картинки
function d4x() {
    pc4x2c.style.display = 'none';
    pc4x2r.style.display = 'none';
    pc4x2l.style.display = 'none';
    pc4x4c.style.display = 'none';
    pc4x4r.style.display = 'none';
    pc4x4l.style.display = 'none';
    if (x4 === 2) {
        if (rot === 1) {
            pc4x2r.style.display = 'block';
        }
        if (rot === 0) {
            pc4x2c.style.display = 'block';
        }
        if (rot === -1) {
            pc4x2l.style.display = 'block';
        }
    }
    if (x4 === 4) {
        if (rot === 1) {
            pc4x4r.style.display = 'block';
        }
        if (rot === 0) {
            pc4x4c.style.display = 'block';
        }
        if (rot === -1) {
            pc4x4l.style.display = 'block';
        }
    }
}

// Функция реакции на нажатие клавиши на клавиатуре
document.addEventListener('keydown', function (e) {
    if (e.key === ' ') {
        if (s_mot == 1) {
            s_mot = 2;
            if (mot === 1) {
                mot = 2;
                mot_set.style.top = '0%';
                mot_set.style.height = '50%';
            }
            if (mot === -1) {
                mot = -2;
                mot_set.style.top = '50%';
                mot_set.style.height = '50%';
            }
            mot_get.style.top = '0%';
            mot_get.style.height = '100%';
            var text_console = 'mot = ' + mot + ' s_mot = ' + s_mot + '\n';
            console.log(text_console);
        }
        else {
            s_mot = 1;
            if (mot === 2) {
                mot = 1;
                mot_set.style.top = '25%';
                mot_set.style.height = '25%';
            }
            if (mot === -2) {
                mot = -1;
                mot_set.style.top = '50%';
                mot_set.style.height = '25%';
            }
            mot_get.style.top = '25%';
            mot_get.style.height = '50%';
            var text_console = 'mot = ' + mot + ' s_mot = ' + s_mot + '\n';
            console.log(text_console);
        }
        d4x();
    }

    if (e.key === 'w') {
        mot = s_mot;
        if (mot === 1) {
            mot_set.style.top = '25%';
            mot_set.style.height = '25%';
        }
        if (mot === 2) {
            mot_set.style.top = '0%';
            mot_set.style.height = '50%';
        }
        var text_console = 'mot = ' + mot + ' s_mot = ' + s_mot + '\n';
        console.log(text_console);
    }
    if (e.key === 's') {
        mot = -s_mot;
        if (mot === -1) {
            mot_set.style.top = '50%';
            mot_set.style.height = '25%';
        }
        if (mot === -2) {
            mot_set.style.top = '50%';
            mot_set.style.height = '50%';
        }
        var text_console = 'mot = ' + mot + ' s_mot = ' + s_mot + '\n';
        console.log(text_console);
    }
    if (e.key === 'd') {
        if (rot === 0) {
            rot = 1;
            var text_console = 'rot = ' + rot + '\n';
            console.log(text_console);
        }
        d4x();
    }
    if (e.key === 'a') {
        if (rot === 0) {
            rot = -1;
            var text_console = 'rot = ' + rot + '\n';
            console.log(text_console);
        }
        d4x();
    }
    if (e.key === 'r') {
        if (x4 === 2) {
            x4 = 4;

            var text_console = '4x4' + rot + '\n';
            console.log(text_console);
        }
        else {
            x4 = 2;
            var text_console = '4x2' + rot + '\n';
            console.log(text_console);
        }
        d4x();
    }
});

// Функция реакции на отпуск клавиши на клавиатуре
document.addEventListener('keyup', function (e) {
    if (e.key === 'w') {
        mot = 0;
        mot_set.style.top = '0%';
        mot_set.style.height = '0%';
        var text_console = 'mot = ' + mot + ' s_mot = ' + s_mot + '\n';
        console.log(text_console);
    }
    if (e.key === 's') {
        mot = 0;
        mot_set.style.top = '0%';
        mot_set.style.height = '0%';
        var text_console = 'mot = ' + mot + ' s_mot = ' + s_mot + '\n';
        console.log(text_console);
    }
    if (e.key === 'd') {
        rot = 0;
        var text_console = 'rot = ' + rot + '\n';
        console.log(text_console);
        d4x();
    }
    if (e.key === 'a') {
        rot = 0;
        var text_console = 'rot = ' + rot + '\n';
        console.log(text_console);
        d4x();
    }
});

// PING
// Необходимые элементы
var p_ping = document.getElementById('ping');
// Функция получения пинга
function ping() {
    var started = new Date().getTime();

    var jsonFile = new XMLHttpRequest();
    jsonFile.open("GET", '/ping', true);
    jsonFile.onreadystatechange = function () {
        if (jsonFile.readyState == 4) {
            var ended = new Date().getTime();
            var milliseconds = ended - started;
            if (milliseconds < 1000) {
                var text_console = 'ping : ' + milliseconds;
                p_ping.innerHTML = text_console;
                p_ping.style.background = '#ffffff';
                p_ping.style.color = '#000000';
                console.log(text_console);
            }
            else {
                var text_console = 'ping : ' + 'ERROR!!!';
                p_ping.innerHTML = text_console;
                p_ping.style.background = '#ff5000';
                p_ping.style.color = '#ffffff';
                console.log(text_console);
            }
        }
    };
    jsonFile.send();
}

setInterval(ping, 100);