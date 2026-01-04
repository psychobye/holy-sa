function log(m) {
    console.log("[CEF_JS] " + m);
    if(window.CefBridge && window.CefBridge.sendClientEvent) {
        window.CefBridge.sendClientEvent("log", JSON.stringify("[JS] " + m));
    }
}

window.CefReady = false;
window.Cef = {
    _events: {},
    
    _trigger(ev, data) {
        log("Trigger: " + ev + " RawData: " + data);
        let p = data;
        if (typeof data === 'string') {
            try { p = JSON.parse(data); } catch(e) { log("Parse Error"); }
        }

        if (ev === "updateHP" && p.hp !== undefined) {
            localStorage.setItem('last_hp', p.hp);
            log("Saved to Storage: " + p.hp);
        }

        if (this._events[ev]) {
            this._events[ev].forEach(cb => cb(p));
        }
    },

    on(ev, cb) {
        if (!this._events[ev]) this._events[ev] = [];
        this._events[ev].push(cb);
        log("Subscribed to: " + ev);
    }
};

function sendInteractiveAreas() {
    if (window.CefBridge && window.CefBridge.updateInteractiveAreas) {
        const rects = [];
        const el = document.getElementById('main-window');

        if (el && window.getComputedStyle(el).display !== 'none') {
            const r = el.getBoundingClientRect();
            rects.push([r.x, r.y, r.width, r.height]);
        }

        window.CefBridge.updateInteractiveAreas(JSON.stringify(rects));
    }
}

window.addEventListener("DOMContentLoaded", () => {
    window.CefReady = true;
    log("CEF Ready");

    if(window.CefBridge && window.CefBridge.cefReady) {
        window.CefBridge.cefReady();
    }
});

window.onresize = sendInteractiveAreas;