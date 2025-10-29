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
        idSetInterval = setInterval(updateStats, 1000);
    }
    let textInput = document.getElementById("textInput");
    if (event.key === " " || event.key === "/" || event.key === "'"){
        event.preventDefault();
        if (document.activeElement === textInput) {
            textInput.value += event.key
        }
    }

    if (document.activeElement === textInput) {
            return
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

function updateStatsHistory(){
    let charsPerSecond = {
        total: charsTyped.total - lastCharsTyped.total,
        correct: charsTyped.correct - lastCharsTyped.correct,
        wrong: charsTyped.wrong - lastCharsTyped.wrong,
    };
    historyChars.push(charsPerSecond);
    lastCharsTyped = {...charsTyped};
}

function updateStatsGroup(groupSize) {
    let typedView = document.getElementById('typedLast' + groupSize);
    let correctView = document.getElementById('correctLast' + groupSize);
    let wrongView = document.getElementById('wrongLast' + groupSize);

    let average = {
        total: 0,
        correct: 0,
        wrong: 0,
    };
    if (groupSize <= historyChars.length) {
        for (i = historyChars.length - 1;
             i >= historyChars.length - groupSize;
             --i){
            average.total += historyChars[i].total;
            average.correct += historyChars[i].correct;
            average.wrong += historyChars[i].wrong;
        }
        average.total /= groupSize;
        average.correct /= groupSize;
        average.wrong /= groupSize;
    }
    typedView.innerHTML = average.total.toFixed(2);
    correctView.innerHTML = average.correct.toFixed(2);
    wrongView.innerHTML = average.wrong.toFixed(2);
}

function updateStatsDisplay() {
    updateStatsGroup(1);
    updateStatsGroup(5);
    updateStatsGroup(30);
}

function updateStats() {
    updateStatsHistory();
    updateStatsDisplay();
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

    updateStatsDisplay()
}


let idSetInterval = -1;
window.addEventListener("focus", () => {
    if (idSetInterval < 0)
        idSetInterval = setInterval(updateStats, 1000);
});

window.addEventListener("blur", () => {
    if (idSetInterval >= 0)
        clearInterval(idSetInterval);
    idSetInterval = -1;
});
