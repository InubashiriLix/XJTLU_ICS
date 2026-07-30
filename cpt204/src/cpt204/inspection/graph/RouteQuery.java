package cpt204.inspection.graph;

import java.util.Arrays;

/**
 * Describes one Task B route query.
 */
public class RouteQuery {
    private final String caseName;
    private final String description;
    private final String startLocation;
    private final String destinationLocation;
    private final String[] waypoints;

    private RouteQuery(
            String caseName,
            String description,
            String startLocation,
            String destinationLocation,
            String[] waypoints
    ) {
        this.caseName = caseName;
        this.description = description;
        this.startLocation = startLocation;
        this.destinationLocation = destinationLocation;
        this.waypoints = Arrays.copyOf(waypoints, waypoints.length);
    }

    public static RouteQuery direct(
            String caseName,
            String description,
            String startLocation,
            String destinationLocation
    ) {
        return new RouteQuery(caseName, description, startLocation, destinationLocation, new String[0]);
    }

    public static RouteQuery via(
            String caseName,
            String description,
            String startLocation,
            String destinationLocation,
            String... waypoints
    ) {
        return new RouteQuery(caseName, description, startLocation, destinationLocation, waypoints);
    }

    public String getCaseName() {
        return caseName;
    }

    public String getDescription() {
        return description;
    }

    public String getStartLocation() {
        return startLocation;
    }

    public String getDestinationLocation() {
        return destinationLocation;
    }

    public String[] getWaypoints() {
        return Arrays.copyOf(waypoints, waypoints.length);
    }

    public String getWaypointsLabel() {
        if (waypoints.length == 0) {
            return "None";
        }
        return String.join(" -> ", waypoints);
    }
}
