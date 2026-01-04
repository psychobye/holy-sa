function renderHP() {
    const fill = document.getElementById("hp-fill");
    const text = document.getElementById("hp-text");
    if (!fill || !text) return;

    const hp = parseFloat(localStorage.getItem('last_hp') || 0);
    const percent = Math.min(Math.max(hp, 0), 100);

    fill.style.width = percent + "%";
    text.innerText = hp.toFixed(1) + " / 100";

    if (percent > 60) fill.style.backgroundColor = "#4cd964";
    else if (percent > 25) fill.style.backgroundColor = "#ffd24d";
    else fill.style.backgroundColor = "#ff4b4b";

    log("UI Sync: " + percent + "%");
}

document.getElementById("send-btn").addEventListener("click", () => {
    if (window.CefBridge) window.CefBridge.sendClientEvent("buttonPressed", "{}");
});

window.Cef.on("updateHP", renderHP);

window.addEventListener("DOMContentLoaded", renderHP);
window.addEventListener("DOMContentLoaded", sendInteractiveAreas);