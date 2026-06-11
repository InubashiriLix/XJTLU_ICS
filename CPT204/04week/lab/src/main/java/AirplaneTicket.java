import java.util.Date;
import java.util.Comparator;

public class AirplaneTicket {
    private String passengerName;
    private int ticketPrice;
    private String flightNumber;
    private Date arrivalTime;
    private SeatClass seatClass;

    public enum SeatClass {
        FIRST_CLASS, BUSINESS, ECONOMY
    };

    public AirplaneTicket(String passengerName,
            int ticketPrice,
            String flightNumber,
            Date arrivalTime,
            SeatClass seatClass) {
        this.passengerName = passengerName;
        this.ticketPrice = ticketPrice;
        this.flightNumber = flightNumber;
        this.arrivalTime = arrivalTime;
        this.seatClass = seatClass;
    }

    public String getPassengerName() {
        return passengerName;
    }

    public int getTicketPrice() {
        return ticketPrice;
    }

    public String getFlightNumber() {
        return flightNumber;
    }

    public Date getArrivalTime() {
        return arrivalTime;
    }

    public SeatClass getSeatClass() {
        return seatClass;
    }

    public static Comparator<AirplaneTicket> getTicketComparator() {
        return new TicketComparator();
    }

    // Complete the class TicketComparator
    private static class TicketComparator implements Comparator<AirplaneTicket> {
        @Override
        public int compare(AirplaneTicket t1, AirplaneTicket t2) {
            // Compare by passenger name first
            int nameComp = t1.getPassengerName().compareTo(t2.getPassengerName());
            if (nameComp != 0) {
                return nameComp;
            }
            // then ticket price
            int priceComp = Integer.compare(t1.getTicketPrice(), t2.getTicketPrice());
            if (priceComp != 0) {
                return priceComp;
            }
            // then arrival time
            int arrivalComp = t1.getArrivalTime().compareTo(t2.getArrivalTime());
            if (arrivalComp != 0)
                return arrivalComp;
            // then seta class
            // we can convert the enum to integer for comparison
            return Integer.compare(t1.getSeatClass().ordinal(), t2.getSeatClass().ordinal());
        }

    }

}
