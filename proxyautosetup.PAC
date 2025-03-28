function FindProxyForURL(url, host) {
    // List of domains to route through the proxy
    var proxyDomains = [
        "udonvr-db.github.io", 
        "example.com", 
        "bluekunvrc.github.io", 
        "vrclinking.com", 
        "linkingbotvrchat.github.io",
		"pastebin.com",
"idlecube-cryo.github.io"
    ];

    // Proxy server address
    var proxy = "PROXY 127.0.0.1:8080";

    // Route a specific URL via proxy
    if (shExpMatch(url, "*pastebin.com*") || 
        shExpMatch(url, "http://pastebin.com/raw/1djvT67N")) {
        return proxy;
    }

    // Check if the host matches any domain in the list
    for (var i = 0; i < proxyDomains.length; i++) {
        if (dnsDomainIs(host, proxyDomains[i])) {
            return proxy;
        }
    }

    // Default: connect directly
    return "DIRECT";
}
