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
	private ServerSocket servSock;
	private Socket sock;
	private Scanner in;
	private PrintWriter out;
	private BufferedReader br;

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
		servSock = new ServerSocket(port);
		System.out.println("Server is connected on port: " + port);
		System.out.println("Waiting for Ms Roboto..");
		sock = servSock.accept(); 
		System.out.println("Ms Roboto has connected");
		in = new Scanner(sock.getInputStream());
		out = new PrintWriter(sock.getOutputStream());
		System.out.println("Connected established with Ms Roboto!! <3");
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
		System.out.println("Stoping server..");
		in.close();
		out.close();
		servSock.close();
		sock.close();
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
	String getRobotoReply() 
	{
		String robotoReply = "";
		if (in.hasNext()) { 
			return in.nextLine();
		}
		return robotoReply;
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
	void sendMsg(String msg) 
	{
		System.out.println("Message sent to Ms Roboto: " + msg);
		out.println(msg);
		System.out.println("Waiting for Ms Roboto to reply");
		if (getRobotoReply().equals(ROBOTO_REPLY))
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