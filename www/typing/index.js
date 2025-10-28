let currentPosition = 0;
let word = "hello";

let charsTyped = {
    total: 0,
    correct: 0,
    wrong: 0,
};

let lastCharsTyped = {
    total: 0,
    correct: 0,
    wrong: 0,
};

let historyChars = [];

function appendWord(word)
{
    console.log("appendWord: ", word);
    let wordsTag = document.getElementById('words');
    wordsTag.innerHTML = "";
    chars = word.split("");

    chars.forEach( (ch, i) => {
        let span = document.createElement("span");
        console.log(span);

        if (ch === " " || ch === "\n" || ch === "\t") {
            span.innerHTML = "&nbsp;";
        } else {
            span.innerHTML = ch;
        }

        span.id = "ch" + i;
        if (i === 0)
        {
            span.classList.add("current");
        }
        wordsTag.appendChild(span);
        if (ch === "\n"){
            br = document.createElement("br");
            wordsTag.appendChild(br);
        }
    })
    // end with space
    span = document.createElement("span");
    span.innerHTML = "&nbsp;";
    span.id = "ch" + chars.length;
    wordsTag.appendChild(span);
    let firstCh = document.getElementById("ch0");
    if (firstCh !== null)
        firstCh.scrollIntoView({ behavior: "instant", block: "start" });
}

appendWord(word);


document.addEventListener("keydown", (event) => {
    if (idSetInterval < 0) {
        idSetInterval = setInterval(countCharsPerSecond, 1000);
    }
    if (event.key === " " || event.key === "/"){
        event.preventDefault();
    }
    //console.log(event);
    if (event.key.length > 1) {
        if (event.key === "Backspace"|| event.key === "Delete"){
            //console.log("delete was pressed");
            // maybe TODO handle support for deleting chars
        } else if (event.key === "Enter"|| event.key === "Tab") {
            //console.log("special space pressed");
        }
    } else {
        if (currentPosition >= word.length) {
            // start again if space input
            if (event.key === " "
                || event.key === "Enter"
                || event.key === "Tab") {
                ++charsTyped.total;
                ++charsTyped.correct;
                currentPosition = 0;
                appendWord(word);
            }
        } else {
            ++charsTyped.total;
            let chSpan = document.getElementById("ch" + currentPosition);
            let currentCh = chSpan.innerHTML;
            currentCh = normalizeCh(currentCh);
            let keyCh = event.key;
            console.log (currentCh, " | ", keyCh);
            if (currentCh === keyCh) {
                chSpan.classList.replace("current", "good");
                ++charsTyped.correct;
            } else {
                chSpan.classList.replace("current", "bad");
                ++charsTyped.wrong;
            }
            ++currentPosition;
            let nextChSpan = document.getElementById("ch" + currentPosition);
            if (nextChSpan !== null) {
                nextChSpan.classList.add("current");
                nextChSpan.scrollIntoView({ behavior: "instant", block: "start" });
            }
        }
    }
});

function normalizeCh(ch) {
    switch(ch) {
    case "&nbsp;" : return " ";
    case "&lt;" : return "<";
    case "&gt;" : return ">";
    case "&amp;" : return "&";
    default : return ch;
    }
}

function countCharsPerSecond() {
    let typedLastSecond = document.getElementById('typedLastSecond');
    let correctLastSecond = document.getElementById('correctLastSecond');
    let wrongLastSecond = document.getElementById('wrongLastSecond');

    charsPerSecond = {
        total: charsTyped.total - lastCharsTyped.total,
        correct: charsTyped.correct - lastCharsTyped.correct,
        wrong: charsTyped.wrong - lastCharsTyped.wrong,
    };
    typedLastSecond.innerHTML = charsPerSecond.total;
    correctLastSecond.innerHTML = charsPerSecond.correct;
    wrongLastSecond.innerHTML = charsPerSecond.wrong;

    historyChars.push(charsPerSecond);
    if (historyChars.length >= 5) {
        let typedLastFiveSecond = document.getElementById('typedLastFiveSecond');
        let correctLastFiveSecond = document.getElementById('correctLastFiveSecond');
        let wrongLastFiveSecond = document.getElementById('wrongLastFiveSecond');

        let averageFiveSec = {
            total: 0,
            correct: 0,
            wrong: 0,
        };
        for (i = historyChars.length - 1; i >= historyChars.length - 5; --i) {
            averageFiveSec.total += historyChars[i].total;
            averageFiveSec.correct += historyChars[i].correct;
            averageFiveSec.wrong += historyChars[i].wrong;
        }
        averageFiveSec.total /= 5;
        averageFiveSec.correct /= 5;
        averageFiveSec.wrong /= 5;
        typedLastFiveSecond.innerHTML = averageFiveSec.total;
        correctLastFiveSecond.innerHTML = averageFiveSec.correct;
        wrongLastFiveSecond.innerHTML = averageFiveSec.wrong;

    }
    if (historyChars.length >= 30) {
        let typedLastThirtySecond = document.getElementById('typedLastThirtySecond');
        let correctLastThirtySecond = document.getElementById('correctLastThirtySecond');
        let wrongLastThirtySecond = document.getElementById('wrongLastThirtySecond');

        let averageThirtySec = {
            total: 0,
            correct: 0,
            wrong: 0,
        };
        for (i = historyChars.length - 1; i >= historyChars.length - 30; --i) {
            averageThirtySec.total += historyChars[i].total;
            averageThirtySec.correct += historyChars[i].correct;
            averageThirtySec.wrong += historyChars[i].wrong;
        }
        averageThirtySec.total /= 30;
        averageThirtySec.correct /= 30;
        averageThirtySec.wrong /= 30;
        typedLastThirtySecond.innerHTML = averageThirtySec.total.toFixed(2);
        correctLastThirtySecond.innerHTML = averageThirtySec.correct.toFixed(2);
        wrongLastThirtySecond.innerHTML = averageThirtySec.wrong.toFixed(2);

    }
    lastCharsTyped = {...charsTyped};
}

function setTextPractice() {
    let textInput = document.getElementById("textInput");
    console.log(textInput);
    let text = textInput.value.trim();
    if (text.length === 0)
        return;
    textInput.value = "";
    initStats();
    word = text;
    appendWord(text);
}

function initStats() {
    currentPosition = 0;
    charsTyped = {
        total: 0,
        correct: 0,
        wrong: 0,
    };

    lastCharsTyped = {
        total: 0,
        correct: 0,
        wrong: 0,
    };

    historyChars = [];

    let typedLastSecond = document.getElementById('typedLastSecond');
    let correctLastSecond = document.getElementById('correctLastSecond');
    let wrongLastSecond = document.getElementById('wrongLastSecond');
    typedLastSecond.innerHTML = 0;
    correctLastSecond.innerHTML = 0;
    wrongLastSecond.innerHTML =  0;

    let typedLastFiveSecond = document.getElementById('typedLastFiveSecond');
    let correctLastFiveSecond = document.getElementById('correctLastFiveSecond');
    let wrongLastFiveSecond = document.getElementById('wrongLastFiveSecond');
    typedLastFiveSecond.innerHTML = 0;
    correctLastFiveSecond.innerHTML = 0;
    wrongLastFiveSecond.innerHTML = 0;

    let typedLastThirtySecond = document.getElementById('typedLastThirtySecond');
    let correctLastThirtySecond = document.getElementById('correctLastThirtySecond');
    let wrongLastThirtySecond = document.getElementById('wrongLastThirtySecond');
    typedLastThirtySecond.innerHTML = 0;
    correctLastThirtySecond.innerHTML = 0;
    wrongLastThirtySecond.innerHTML = 0;
}

let idSetInterval = -1;
window.addEventListener("focus", () => {
    if (idSetInterval < 0)
        idSetInterval = setInterval(countCharsPerSecond, 1000);
});

window.addEventListener("blur", () => {
    if (idSetInterval >= 0)
        clearInterval(idSetInterval);
    idSetInterval = -1;
});
