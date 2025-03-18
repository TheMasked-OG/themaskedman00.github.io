function FindProxyForURL(url, host) {
    // List of domains or URLs to route through the proxy
    var proxyDomains = ["udonvr-db.github.io", "example.com", "bluekunvrc.github.io", "vrclinking.com"];

    // Proxy server address (change to your proxy IP and port)
    var proxy = "PROXY 127.0.0.1:8080";  // Adjust the IP and port for your proxy

    // Check if the current host is in the list of proxy domains
    for (var i = 0; i < proxyDomains.length; i++) {
        if (dnsDomainIs(host, proxyDomains[i])) {
            return proxy;  // Route through the proxy for matching domains
        }
    }

    // If the URL doesn't match, bypass the proxy
    return "DIRECT";
}
