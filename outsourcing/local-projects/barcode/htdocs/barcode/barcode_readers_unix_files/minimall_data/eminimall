var ContentUtil =
{
	elements: null,

	trim: function( s )
	{
		s = new String( s );
		s = s.replace( /^ +/g, "" );
		s = s.replace( / +$/g, "" );
		
		return s;		
	},

	addStyle: function( element, style )
	{
		if( element != null ) {
			var styles = new String( style ).split( ";" );
			for( var i = 0; i < styles.length; i++ ) {
				var nvp = new String( styles[i] ).split( ":" );
				if( nvp != null ) {
					if( nvp.length > 1 ) {
						var name  = ContentUtil.trim( nvp[0] );
						var value = ContentUtil.trim( nvp[1] );
						element.style[name] = value;
					}
				}
			}
		}
	},

	getElementsByClassName: function( className )
	{
		ContentUtil.elements = [];
		var elements = null;
		if( document.all ) {
			elements = document.all;
		} else
		if( document.getElementsByTagName ) {
			elements = document.getElementsByTagName( "*" );
		}

		for( var i = 0; i < elements.length; i++ ) {
			var element = elements[i];
			if( element != null ) {
				if( element.className == className ) {
					var elements = ContentUtil.elements;
					elements[elements.length] = element;
				}
			}
		}

		return ContentUtil.elements;
	},

	getContentById: function( id )
	{
		var element = document.getElementById( id );
		return element ? element.innerHTML : "";
	},
	
	getOuterHTML: function( element )
	{
		// these are the elements we care about
		var properties = { "name": "name", "id": "id", "class": "class" };
		var buffer = "<" + element.tagName;

		// get the attributes
		for( var property in properties ) {
			var value = null;
			switch( property ) {
				case "class" :
					value = element.className;
				break;
				default:
					value = element[property];
				break;
			}
			if( ( value != null ) && ( value != "" ) ) {
				buffer += " " + property + "='" + value + "'";
			}
		}

		// get the innterHTML
		buffer += ">";
		buffer += element.innerHTML;

		// close the tag
		buffer += "</" + element.tagName + ">";
		
		return buffer;
	}
}

var JSONUtil =
{
	hasKey: function( obj, name )
	{
		for( var k in obj ) {
			if( k == name ) {
				return true;
			}
			if( typeof( obj[k] ) == "object" ) {
				JSONUtil.hasKey( obj[k], name );
			}
		}
		return false;
	}
}

var EMiniMall =
{
	// will become the list of displaying tabs
	tabnames: [],
	
	// if no tabs are specified in config, these are the ones we'll use
	defaultTabs: [ "Description", "Reviews", "Search" ],
	
	display: function( data )
	{
		EMiniMall.tabnames = config["tabs"] ? config["tabs"] : EMiniMall.defaultTabs;

		var tabWidth  = config["tab_width"] ? config["tab_width"] : 70;
		var tabHeight = config["tab_height"] ? config["tab_height"] : 25;
		var tabPanelWidth  = config["tab_panel_width"] ? config["tab_panel_width"] : 320;
		var tabPanelHeight = config["tab_panel_height"] ? config["tab_panel_height"] : 125;

		// Set the tab images first
		var tabs = new TabPanel( "mmTabs", tabPanelWidth, tabPanelHeight, tabWidth, tabHeight );
		tabs.setBackgroundImages();

		// determine which tabs we can show based on the data we have
		var showTabs = [];
		for( var i = 0; i < EMiniMall.tabnames.length; i++ ) {
			var tabname = EMiniMall.normalizeName( EMiniMall.tabnames[i] );
			var miniMallTab = EMiniMallTabs[tabname];
			if( miniMallTab ) {
				var meetsDependencies = EMiniMall.meetsDependencies( miniMallTab, data );
				if( meetsDependencies ) {
					if( miniMallTab.render(data) != null ) {
						showTabs[showTabs.length] = EMiniMall.tabnames[i];
					}
				}
			}
		}
				
		// always show the eminimalls tab last
		var showMiniMalls = true;
		if( config["exclude_tabs"] ) {
			var exclude_tabs = config["exclude_tabs"];
			for( var i = 0; i < exclude_tabs.length; i++ ) {
				var tabname = exclude_tabs[i];
				if( tabname == "eMiniMalls" ) {
					showMiniMalls = false;
					break;
				}
			}
		}
		
		if( showMiniMalls == true ) {
			showTabs[showTabs.length] = "eMiniMalls";
		}

		EMiniMall.tabnames = showTabs;

		for( var i = 0; i < showTabs.length; i++ ) {
			tabs.addTab( showTabs[i] );
		}

		EMiniMall.render( data );
		EMiniMall.addCustomStyles();

		for( var i = 0; i < showTabs.length; i++ ) {
			var tabname = EMiniMall.normalizeName( showTabs[i] );
			var id = "mm" + tabname;
			tabs.setPanel( showTabs[i], ContentUtil.getContentById( id ) );
		}

		tabs.display();

		// EMiniMall.animate( data );
	},

	addCustomStyles: function()
	{
		if (typeof styles == 'undefined') return; 

		// if styles are defined for an ID
		for( var id in styles ) {
			var element = document.getElementById(id);
			var style = styles[id];
			if( element != null ) { 
				ContentUtil.addStyle( element, style );
			}
		}
		
		// if styles are defined for classNames
		for( var className in styles ) {
			var elements = ContentUtil.getElementsByClassName( className );
			for( var i = 0; i < elements.length; i++ ) {
				var element = elements[i];
			    var style = styles[className];
			    if( element != null ) {
					ContentUtil.addStyle( element, style );
				}
			}		
		}
	},
	
	normalizeName: function( name )
	{
		return new String( name ).toLowerCase().replace( / /g, "_" );
	},
	
	meetsDependencies: function( miniMallTab, data )
	{
		// do we have all the data required for this tab?
		// for example, if it is the recommendation tab, and
		// there are no recommendations, we don't meet dependencies.
		var meetsDependency = true;
		var dependencies = miniMallTab.dependencies;
		for( var j = 0; j < dependencies.length; j++ ) {
			var dependency = dependencies[j];
			meetsDependency = JSONUtil.hasKey( data, dependency );
			if( meetsDependency != true ) {
				break;
			}
		}
		
		return meetsDependency;
	},

	render: function( data )
	{
		var tabnames = EMiniMall.tabnames;
		
		for( var i = 0; i < tabnames.length; i++ ) {
			var tabname = tabnames[i].toLowerCase().replace( / /g, "_" );
			var miniMallTab = EMiniMallTabs[tabname];
			if( miniMallTab != null ) {
				var tabElement = document.createElement( "span" );
				tabElement.id = "mm" + tabname;
				tabElement.style.display = "none";
				//tabElement.appendChild( miniMallTab.render( data ) );
				tabElement.innerHTML = miniMallTab.render( data );
				document.body.appendChild( tabElement );
			}
		}

		// create unit
		var unit = document.createElement( "div" );
		unit.className = "mm";
		
		var logo = document.createElement( "img" );
		logo.src = mm_global_location + "/eminimalls/global/images/clogo.gif";
		logo.border = 0;

		var chitikaLink = document.createElement( "a" );
		chitikaLink.target = "_blank";
		chitikaLink.href = "http://www.chitika.com/eminimalls";
		chitikaLink.className = "mmLogo";
		chitikaLink.id = "mmLogo";
		chitikaLink.target = "_new";
		chitikaLink.title = "eMiniMalls by Chitika";
		chitikaLink.appendChild( logo );

		var tooltip = data["product"]["name"];
		var vendors = data["vendors"];
		var vendorCount = vendors ? vendors.length : 0;
		if( vendorCount > 0 ) {
			var vendor = vendors[0];
			tooltip += " -- Available at " + vendor["name"] + " for " + vendor["price"];
		}

		var img = document.createElement( "img" );
		img.border = "0";
		img.id = "mmProductImage";
		if ( data["product"] && data["product"]["image"] ) {
			img.src = data["product"]["image"];
		} else {
			img.src = mm_global_location + "/eminimalls/global/images/clear.gif";
		}
		img.className = "mmProductImage";
		// img.style["filter"] = "progid:DXImageTransform.Microsoft.fade(duration=2)";
		img.title = tooltip;
		
		var tabContainer = document.createElement( "div" );
		tabContainer.id = "mmTabs";
		tabContainer.className = "mmTabs";
		
		unit.appendChild( img );
		unit.appendChild( chitikaLink );
		unit.appendChild( tabContainer );
		
		var border = document.createElement( "div" );
		border.id = "mmBorder";
		border.className = "mmBorder";
		document.body.appendChild( border );
		document.body.appendChild( unit );
	},

	animate: function( data )
	{
		setTimeout( EMiniMall.zoom, 0 );
		setTimeout( "EMiniMall.fade('" + data["product"]["image"] + "')", 0 );
	},

	i:75,
	wait: 75,
	zoom: function()
	{
		var mmProductImage = document.getElementById( "mmProductImage" );
		if( mmProductImage ) {
			mmProductImage["style"].zoom = "." + EMiniMall.i++;
			if( EMiniMall.i < 100 ) {
				EMiniMall.wait = EMiniMall.i < 90 ? EMiniMall.wait : 100;
				setTimeout( EMiniMall.zoom, EMiniMall.wait );
			}
		}
	},
	
	fade: function( image_url )
	{
		var mmProductImage = document.getElementById( "mmProductImage" );
		if( mmProductImage ) {
			if( mmProductImage.filters ) {
				mmProductImage.filters[0].Apply();
				mmProductImage.src = image_url;
				mmProductImage.filters[0].Play()
			} else {
				mmProductImage.src = image_url;
			}
		}
	}
}

//////////
//
// name: TabUtil
// desc: A static class for manipulating Tab related classes and
//       handling Tab related events.
//

var TabUtil =
{
	//////////
	//
	// name: onMouseOver
	// desc: The onmouseover event handler for a tab. This
	//       handler sets the tab's className. The class gives
	//       the tab the look of a hover tab.
	//

	onMouseOver: function( tabElement )
	{
		if( tabElement.className != "tabSelected" ) {
			tabElement.className = "tabHover";
		}
		TabUtil.onClick( tabElement );
	},

	//////////
	//
	// name: onMouseOut
	// desc: The onmouseover event handler for a tab. This
	//       handler sets the tab's className. The class gives
	//       the tab the look of a default tab.
	//

	onMouseOut: function( tabElement )
	{
		if( tabElement.className == "tabHover" ) {
			tabElement.className = "tabDefault";
		}
	},

	//////////
	//
	// name: onClick
	// desc: The onclick event handler for a tab. This
	//       handler selects the clicked tab and shows
	//       the corresponding tab panel.
	//

	onClick: function( tabElement )
	{
		TabUtil.selectTab( tabElement );
		TabUtil.showTabPanel( tabElement );
	},

	//////////
	//
	// name: selectTab
	// desc: Given a tab DOM element, find its sibling tabs,
	//       make them default tabs, and make the passed tab the
	//       selected tab.
	//

	selectTab: function( tabElement )
	{
		var parent = tabElement.parentNode;
		for( var i = 0; i < parent.childNodes.length; i++ ) {
			var child = parent.childNodes[i];
			child.className = "tabDefault";
			child.style.zIndex = TabUtil.zIndexTab;
		}
		
		tabElement.className = "tabSelected";
		tabElement.style.zIndex = TabUtil.zIndexTab + 1;
	},

	//////////
	//
	// name: getTabPanel
	// desc: Given a tab DOM element, get the corresponding
	//       tab panel DOM element
	//

	getTabPanel: function( tabElement )
	{
		// given a tab, get the coresponding tab pabel
		var tabId = tabElement.id;
		var panId = "tabPanel" + tabId.replace( "tab", "" );
		var element = document.getElementById( panId );
		
		return element;
	},
	
	//////////
	//
	// name: showTabPanel
	// desc: Given a tab DOM element, we find its corresponding
	//       tabPanel, display it, and hide the other panels. We
	//		 need to hide the other panels (as opposed to just
	//       setting its z-index) to fix an IE bug that causes
	//       form elements to "show through"
	//

	showTabPanel: function( tabElement )
	{
		var panel = TabUtil.getTabPanel( tabElement );
		var parent = panel.parentNode;
		for( var i = 0; i < parent.childNodes.length; i++ ) {
			var child = parent.childNodes[i];
			child.style.display = 'none';
		}
		
		panel.style.zIndex = TabUtil.zIndexTab;
		panel.style.display = 'inline'
	},

	zIndexTab: 1
}

function TabPanel( name, width, height, tabwidth, tabheight )
{
	this.display	 = __TabPanel_display;
	this.addTab		 = __TabPanel_addTab;
	this.setPanel	 = __TabPanel_setPanel;
	this.getPanel    = __TabPanel_getPanel;
	this.getPanels   = __TabPanel_getPanels;
	this.hidePanels  = __TabPanel_hidePanels;
	this.setBackgroundImages  = __TabPanel_setBackgroundImages;

	this.name	   = name;
	this.width	   = width;
	this.height	   = height;
	this.tabwidth  = tabwidth;
	this.tabheight = tabheight;

	this.tabs = new Array();
	this.panels = new Array();
	this.tabcount = 0;
}

function __TabPanel_setBackgroundImages( )
{
	// ugh ... there's no way to do CSS variables ... so we'll
	// have to set variable based CSS values here in a style block
	var tabWidth = config["tab_width"] ? config["tab_width"] : 70;
	var tabHeight = config["tab_height"] ? config["tab_height"] : 25;

	document.write( "<" + "style" + ">" );
	document.write( ".tabDefault " );
	document.write( "{ " );
	document.write( "background: url('" + mm_global_location + "/eminimalls/global/images/" + tabWidth + "x" + tabHeight + ".tab.v3.png') no-repeat top;" );
	document.write( "} " );

	document.write( ".tabHover " );
	document.write( "{ " );
	document.write( "background: url('" + mm_global_location + "/eminimalls/global/images/" + tabWidth + "x" + tabHeight + ".tab.hover.v3.png') no-repeat top;" );
	document.write( "} " );

	document.write( ".tabSelected " );
	document.write( "{ " );
	document.write( "background: url('" + mm_global_location + "/eminimalls/global/images/" + tabWidth + "x" + tabHeight + ".tab.active.v3.png') no-repeat top;" );
	document.write( "} " );

	document.write( "</" + "style" + ">" );
}

function __TabPanel_addTab( name )
{
	this.tabs[name] = this.tabcount++;
}

function __TabPanel_display()
{
	var tabs = '';
	var style = "";
	
	style += "width: " + this.width + "px; ";
	style += "height: " + this.height + "px; ";
	tabs += '<div style="' + style + '" id="' + this.name + 'Group" class="tabPanelGroup">';
	tabs += '<div class="tabGroup">';

	// adds tabs
	var tabPosition = 0;
	for( var name in this.tabs ) {
		var type = ( tabPosition == 0 ) ? "tabSelected" : "tabDefault";
		var zIndex = ( tabPosition == 0 ) ? TabUtil.zIndexTab : 0;
		var id = "tab" + tabPosition;
		var style = "left:" + ( this.tabwidth * tabPosition ) + ";";
		style += "z-index:" + zIndex + ";"
		tabs += '<div onmouseover="TabUtil.onMouseOver(this)" onmouseout="TabUtil.onMouseOut(this)" style="' + style + '" id="' + id + '" class="' + type + '" onclick="TabUtil.onClick(this)"><div id="tabLabel" class="tabLabel">' + name + '</div></div>';
		tabPosition++;
	}

	tabs += '</div>';

	// adds tab panels
	tabs += '<div>';
	var panelPosition = 0;
	for( var name in this.panels ) {
		var id = "tabPanel" + panelPosition;
		var data = this.panels[name];
		var style = "display: " + (( panelPosition == 0 ) ? "inline" : "none" );
		style += "; height: " + (this.height - this.tabheight ) + "px; ";
		style += "width: " + this.width + "px; "
		style += "top: " + ( this.tabheight - 1 ) + "px;"
		tabs += '<div style="' + style + '" id="' + id + '" class="tabPanel">' + data + '</div>';
		panelPosition++;
	}
	tabs += '</div>';

	tabs += '</div>';
	
	var container = document.getElementById( this.name );
	if( container == null ) {
		container = document.createElement( "div" );
		container.setAttribute( "id", this.name );
		document.body.appendChild( container );
	}

	container.innerHTML = tabs;
}

function __TabPanel_setPanel( name, data )
{
	this.panels[name] = data;
}

function __TabPanel_hidePanels()
{
	var panels = this.getPanels();
	
	for( var i = 0; i < panels.length; i++ ) {
		var panel = panels[i];
		panel.style.display = 'none';
	}
}

function __TabPanel_getPanels()
{
	var panelPosition = 0;
	var panels = new Array();
	for( var n in this.panels ) {
		var id = "tabPanel" + panelPosition;
		panels[panelPosition] = document.getElementById( id );
		panelPosition++;
	}

	return pannels;
}

function __TabPanel_getPanel( name )
{
	var panelPosition = 0;
	for( var n in this.panels ) {
		if( n == name ) {
			var id = "tabPanel" + panelPosition;
			return document.getElementById( id );
		}
		panelPosition++;
	}
}
