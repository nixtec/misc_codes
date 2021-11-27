// phpads.js
//
// Show ad from phpadserver.

function phpads_show_ad(adserver, adzone)
{
     if ( !document.phpAds_used ) document.phpAds_used = ',';
     phpAds_random = new String(Math.random());
     phpAds_random = phpAds_random.substring(2,11);

     document.write("<" + "script type='text/javascript' src='");
     document.write(adserver + "/adjs.php?n=" + phpAds_random);
     document.write("&amp;what=zone:" + adzone);
     document.write("&amp;exclude=" + document.phpAds_used);
     if (document.referer) document.write("&amp;referer=" + escape(document.referer));
     document.write ("'><" + "/script>");
}
