// local.js
//
// Local javascript functions.


//--------------------------------------------------------------------
// Jump to the value of an option in a select.
function jump_to_select_option(sel)
{
	var opt_key = sel.selectedIndex;
	if ( opt_key > 0 ) {
		var uri_val = sel.options[opt_key].value;
		window.open(uri_val, '_top');
	}
	return true;
}

//--------------------------------------------------------------------
function get_left(e)
{
	return e.offsetParent ? e.offsetLeft + get_left(e.offsetParent) : e.offsetLeft;
}

//--------------------------------------------------------------------
function get_top(e)
{
	return e.offsetParent ? e.offsetTop + get_top(e.offsetParent) : e.offsetTop;
}

//--------------------------------------------------------------------
// Show title of event object in the status window.
function show_title(event)
{
	window.status = event.target.title;
	return true;
}

//--------------------------------------------------------------------
// Show balloon help.
function show_balloon(id, event)
{
	if ( browser.dom  &&  !browser.ie ) {
		var elem = document.getElementById(id);
		elem.style.left = event.pageX + 100 + 'px';
		elem.style.top = get_top(event.target) + 'px';
		elem.style.visibility = "visible";
		window.status = event.target.title;
	}
	else if ( browser.ns4 ) {
		document.layers[id].left = event.pageX + 10;
		document.layers[id].top = event.pageY + 10;
		document.layers[id].visibility = "show";
	}
	else if ( browser.ie ) {
		document.all[id].style.pixelLeft = (document.body.scrollLeft + event.clientX) + 10;
		document.all[id].style.pixelTop = (document.body.scrollTop + event.clientY) + 10;
		document.all[id].style.visibility = "visible";
	}
	return true;
}

//--------------------------------------------------------------------
// Hide balloon help.
function hide_balloon(id)
{
	if ( DOM  &&  !IE ) {
		var elem = document.getElementById(id);
		elem.style.visibility = "hidden";
	}
	else if ( NS4 ) {
		document.layers[id].visibility="hide";
	}
	else if ( IE ) {
		document.all[id].style.visibility="hidden";
	}
	return true;
}
