import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;
import java.util.Scanner;

/**
 * A simple UDP server. Receives requests and responds with an echo.
 */
public class SimpleUDPServer {
    public static final int DATA_LENGTH = 1000;
    public static final int DEFAULT_PORT = 35984;
    public static final int MAX_PORT = 65535;
    public static final int MIN_PORT = 0;
    public static final String RESPONSE_HEADER = "echo:";
    public static final byte[] RESPONSE_HEADER_BYTES = RESPONSE_HEADER.getBytes();
    public static final String SOCKET_CLOSED = "Socket closed";

    public static void main(String[] args) {
        final DatagramSocket socket = createSocket(args);
        listen(socket);

        Scanner in = new Scanner(System.in);
        System.out.print("Press enter to exit. ");
        in.nextLine();

        in.close();
        socket.close();
    }

    private static void listen(final DatagramSocket socket) {
        new Thread() {
            @Override
            public void run() {
                System.out.println("Listening on UPD port " + socket.getLocalPort() + ".");
                byte[] data = new byte[DATA_LENGTH];
                while (true) {
                    DatagramPacket packet = new DatagramPacket(data, data.length);
                    try {
                        socket.receive(packet);
                    } catch (IOException e) {
                        if (e instanceof SocketException && SOCKET_CLOSED.equals(e.getMessage())) {
                            // Normal, socket was closed.
                            return;
                        }
                        System.err.println("Error receiving packet:");
                        e.printStackTrace();
                        return;
                    }
                    System.out.println("Packet received from " + packet.getSocketAddress() + ":");
                    System.out.println(new String(data, 0, packet.getLength()));
                    byte[] responseData = new byte[RESPONSE_HEADER.length() + packet.getLength()];
                    System.arraycopy(RESPONSE_HEADER_BYTES, 0, responseData, 0, RESPONSE_HEADER_BYTES.length);
                    System.arraycopy(data, 0, responseData, RESPONSE_HEADER_BYTES.length, packet.getLength());
                    try {
                        DatagramPacket response = new DatagramPacket(responseData, responseData.length, packet.getSocketAddress());
                        socket.send(response);
                    } catch (IOException e) {
                        System.err.println("Error creating or sending response. Not sending.");
                        e.printStackTrace();
                        continue;
                    }
                }
            }
        }.start();
    }

    private static DatagramSocket createSocket(String[] args) {
        int port = getPort(args);
        DatagramSocket socket = null;
        try {
            socket = new DatagramSocket(port);
        } catch (SocketException e) {
            System.err.println("Error creating socket:");
            e.printStackTrace();
            System.exit(1);
        }
        return socket;
    }

    private static int getPort(String[] args) {
        int port = DEFAULT_PORT;
        if (args.length > 0) {
            try {
                port = Integer.parseInt(args[0]);
            } catch (NumberFormatException e) {
                System.err.println("Error parsing port: '" + args[0] + "'.");
                System.err.println("USAGE: java SimpleUDPServer [port]");
                System.exit(1);
            }
            if (port < MIN_PORT || port > MAX_PORT) {
                System.err.println("Error: illegal port: " + port);
                System.err.println("Port must be in range " + MIN_PORT + " to " + MAX_PORT);
                System.exit(1);
            }
        }
        return port;
    }
}
