// browser.js
//
// Setup browser information, taken from phplayersmenu.


var browser = new Object;

browser.dom = (document.getElementById) ? 1 : 0;
browser.ns4 = (document.layers) ? 1 : 0;

// We need to explicitly detect Konqueror because Konqueror 3 sets IE = 1 ... AAAAAAAAAARGHHH!!!
// We need to detect Konqueror 2.2 as it does not handle the window.onresize event
// We need to detect Konqueror 3.2 and 3.3 as they are affected by the see-through effect only for 2 form elements
browser.konqueror = (navigator.userAgent.indexOf('Konqueror') > -1) ? 1 : 0;
browser.konqueror22 = (navigator.userAgent.indexOf('Konqueror 2.2') > -1
					|| navigator.userAgent.indexOf('Konqueror/2.2') > -1) ? 1 : 0;
browser.konqueror30 = (navigator.userAgent.indexOf('Konqueror 3.0') > -1
					|| navigator.userAgent.indexOf('Konqueror/3.0') > -1
					|| navigator.userAgent.indexOf('Konqueror 3;') > -1
					|| navigator.userAgent.indexOf('Konqueror/3;') > -1
					|| navigator.userAgent.indexOf('Konqueror 3)') > -1
					|| navigator.userAgent.indexOf('Konqueror/3)') > -1) ? 1 : 0;
browser.konqueror31 = (navigator.userAgent.indexOf('Konqueror 3.1') > -1
					|| navigator.userAgent.indexOf('Konqueror/3.1') > -1) ? 1 : 0;
browser.konqueror32 = (navigator.userAgent.indexOf('Konqueror 3.2') > -1
					|| navigator.userAgent.indexOf('Konqueror/3.2') > -1) ? 1 : 0;
browser.konqueror33 = (navigator.userAgent.indexOf('Konqueror 3.3') > -1
					|| navigator.userAgent.indexOf('Konqueror/3.3') > -1) ? 1 : 0;

browser.opera = (navigator.userAgent.indexOf('Opera') > -1) ? 1 : 0;
browser.opera5 = (navigator.userAgent.indexOf('Opera 5') > -1
				|| navigator.userAgent.indexOf('Opera/5') > -1) ? 1 : 0;
browser.opera6 = (navigator.userAgent.indexOf('Opera 6') > -1
				|| navigator.userAgent.indexOf('Opera/6') > -1) ? 1 : 0;
browser.opera56 = browser.opera5 || browser.opera6;

browser.ie = (navigator.userAgent.indexOf('MSIE') > -1) ? 1 : 0;
browser.ie = browser.ie && !browser.opera;
browser.ie5 = browser.ie  &&  browser.dom;
browser.ie4 = (document.all) ? 1 : 0;
browser.ie4 = browser.ie4  &&  browser.ie  &&  !browser.dom;
