

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