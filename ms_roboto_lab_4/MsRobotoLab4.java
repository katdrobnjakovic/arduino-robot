/************************************************************************
*
* Names: Craig Bryan
*        Kat Drobnjakovic
*
* Course Code: SEG 4145
* Lab Number: 4
* File name: MsRobotoLab4
* Date: March 26, 2015
*
*
* Description
* *************
* Establish a connection to a WiFi network using the robot.
* Use a PC to exchange data with the robot and execute various commands in real-time.
*
*************************************************************************/ 

import java.io.*;
import java.net.*;
import java.util.*;

class MsRobotoLab4
{
  //Declare Variables

  // Server 
	public final static int PORT = 32114; //random port
	public final static String ROBOTO_REPLY = "d";
	public final static String ROBOTO_ERROR = "e";
	private DatagramSocket servSock;
	private Socket sock;
	private BufferedReader br;

	// Robot IP stuff
	public final static String ROBOT_IP = "10.136.160.16";

	//Character commands for communication protocol
	public final static String FORWARD = "f";
	public final static String BACKWARD = "b";
	public final static String TURN_RIGHT = "r";
	public final static String TURN_LEFT = "l";
	public final static String DISTANCE = "p";
	public final static String TEMPERATURE = "t";
	
/************************************************************************
*
* Name
* *************
* displayInstructions
*
* Description
* *************
* Displays the starting commands. 
*
* Parameters
* *************
* None
*
* Returns
* *************
* void
*
*
*************************************************************************/ 
    static void displayInstructions()
    {
    	System.out.println("Enter the correct number to select an operation:");
        System.out.println("1 – Move the robot forward.");
        System.out.println("2 – Move the robot backward.");
        System.out.println("3 – Rotate the robot clockwise.");
        System.out.println("4 – Rotate the robot counter clockwise.");
        System.out.println("5 – Read the distance to the nearest object.");
        System.out.println("6 – Read temperature values.");
        System.out.println("7 – Quit.");
    }
/************************************************************************
*
* Name
* *************
* startConnection
*
* Description
* *************
* Starts the socket connection and waits for the robot to connect. 
*
* Parameters
* *************
* Port ID
*
* Returns
* *************
* void
*
*
*************************************************************************/ 
  void startConnection(int port) throws IOException 
  {
		System.out.println("Starting server..");
		servSock = new DatagramSocket(port);
		System.out.println("Server is listening on port: " + port);
		System.out.println("Waiting for Ms Roboto..");
		System.out.println(receive());
		System.out.println("Ms Roboto has connected");
		System.out.println("Connected established with Ms Roboto!! <3");
	}


public String receive() throws IOException {
	byte[] data = new byte[1024];
	DatagramPacket receivePacket = new DatagramPacket(data, data.length);
	servSock.receive(receivePacket);
	return receivePacket.getData().toString();
}

public void send(String msg) throws IOException {
	byte[] data = new byte[1024]; 
	data = msg.getBytes();
	DatagramPacket sendPacket = new DatagramPacket(
		data, 
		data.length, 
		InetAddress.getByName(ROBOT_IP), 
		PORT
	);
	servSock.send(sendPacket);
}

/************************************************************************
*
* Name
* *************
* stopConnection
*
* Description
* *************
* Stops the socket and buffer reader. 
*
* Parameters
* *************
* None
*
* Returns
* *************
* void
*
*
*************************************************************************/ 
	void stopConnection() throws IOException 
	{
		System.out.println("Stopping server..");
		br.close();
	}
/************************************************************************
*
* Name
* *************
* distanceInput
*
* Description
* *************
* Reads the inputed distance for forward and backwards traversal from the buffer.
*
* Parameters
* *************
* None
*
* Returns
* *************
* int - the distance in cm (must be 0-20cm)
*
*
*************************************************************************/ 
	int distanceInput() 
	{
		int distance = -1;
		while (distance < 0 || distance > 20) {
			System.out.println("Enter the distance for traversal (0-20cm):");
			try {
				distance = Integer.parseInt(br.readLine());
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		return distance;
	}
/************************************************************************
*
* Name
* *************
* degreeInput
*
* Description
* *************
* Reads the inputed degree for clockwise and counter clockwise rotation from the buffer.
*
* Parameters
* *************
* None
*
* Returns
* *************
* int - the degree (must be 0-359 degrees)
*
*
*************************************************************************/ 
	int degreeInput() 
	{
		int degrees = -1;
		while (degrees < 0 || degrees > 359) {
			System.out.println("Enter the degree of rotation (0-359 degrees):");
			try {
				degrees = Integer.parseInt(br.readLine());
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		return degrees;
	}
/************************************************************************
*
* Name
* *************
* getRobotoReply
*
* Description
* *************
* Gets the message to Ms Roboto
*
* Parameters
* *************
* None
*
* Returns
* *************
* String - the reply from Ms Roboto
*
*
*************************************************************************/ 
	String getRobotoReply(String resp) 
	{
		return resp.substring(0, 1);
	}
/************************************************************************
*
* Name
* *************
* sendMsg
*
* Description
* *************
* Sends the message to Ms Roboto
*
* Parameters
* *************
* None
*
* Returns
* *************
* void
*
*
*************************************************************************/ 
	void sendMsg(String msg) throws IOException {
		System.out.println("Message sent to Ms Roboto: " + msg);
		send(msg);
		System.out.println("Waiting for Ms Roboto to reply");
		String response = receive();
		if (getRobotoReply(response).equals(ROBOTO_REPLY))
			System.out.println("Done!");
		else //ROBOTO_ERROR
			System.out.println("No Bueno!");
	}

/************************************************************************
*
* Name
* *************
* main
*
* Description
* *************
* Starts the server connection on a specified port. 
* Calls the displayInstructions to list the available commands. 
* Gets the input and calls the appropriate functions based on the requested command. 
* Closes the connection.
*
* Parameters
* *************
* None
*
* Returns
* *************
* void
*
*
*************************************************************************/ 
    public static void main ( String[] args )
    {
      try {
				MsRobotoLab4 robotoServer = new MsRobotoLab4();
				robotoServer.startConnection(PORT);
				robotoServer.br = new BufferedReader(new InputStreamReader(System.in));
				int distance = -1;
				int degrees = -1;

				while (true) {
					displayInstructions();
					int resInput = Integer.parseInt(robotoServer.br.readLine());
					if (resInput == 1) {
						distance = robotoServer.distanceInput();
						robotoServer.sendMsg(FORWARD + Integer.toString(distance));
					}
					else if (resInput == 2) {
						distance = robotoServer.distanceInput();
						robotoServer.sendMsg(BACKWARD + Integer.toString(distance));
					} 
					else if (resInput == 3) {
						degrees = robotoServer.degreeInput();
						robotoServer.sendMsg(TURN_RIGHT + Integer.toString(degrees));
					} 
					else if (resInput == 4) {
						degrees = robotoServer.degreeInput();
						robotoServer.sendMsg(TURN_LEFT + Integer.toString(degrees));
					} 
					else if (resInput == 5) {
						robotoServer.sendMsg(DISTANCE);
					} else if (resInput == 6) {
						robotoServer.sendMsg(TEMPERATURE);
					} 
					else {// quit - input = 7
						break;
					}
				}
			
				robotoServer.stopConnection();
			} catch (Exception e) {
				e.printStackTrace();
			}

      	
    }
}