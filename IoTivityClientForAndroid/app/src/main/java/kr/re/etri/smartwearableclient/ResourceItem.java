package kr.re.etri.smartwearableclient;

import java.util.HashMap;

class ResourceItem {
    private String deviceType;
    private String resourceType;
    private String uri;
    private HashMap<String, String> params;
    private String method;

    ResourceItem(String deviceType, String resourceType, String resourceUri, HashMap<String, String> params, String method) {
        this.deviceType = deviceType;
        this.resourceType = resourceType;
        this.uri = resourceUri;

        if(params != null) {
            this.params = params;
        } else {
            this.params = new HashMap<>();
        }

        this.method = method;
    }

    ResourceItem(String deviceType, String resourceType, String resourceUri, HashMap<String, String> params) {
        this(deviceType, resourceType, resourceUri, params, "GET");
    }

    String getDeviceType() { return this.deviceType; }
    String getResourceType() {
        return this.resourceType;
    }
    String getUri() {
        return this.uri;
    }
    HashMap<String, String> getParams() { return this.params; }
    String getMethod() {
        return this.method;
    }
}
