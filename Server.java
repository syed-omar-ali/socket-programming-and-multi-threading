import java.io.*;
import java.net.*;
import java.lang.*;
class Ints{
       public int r1=0,r2=0,c1=0,c2=0;
}
class ComputeAnElement extends Thread{
	int mat1[][],mat2[][],result [][],row,col;
	Ints i;
	public ComputeAnElement(int result[][],Ints i,int row,int col,int mat1[][],int mat2[][]){
		this.result = result;
		this.i = i;
		this.row = row;
		this.col = col;
		this.mat1 = mat1;
		this.mat2 = mat2;
	}
	public void run(){
		System.out.print("\nComputing element ["+row+"]"+"["+col+"]:");
		int idx = 0;
		int sum = 0;
		while(idx<i.c1){
			sum += mat1[row][idx]*mat2[idx][col];
			idx++;
		}
		result[row][col] = sum;
		System.out.println(" "+sum);
	}
	public int[][] updateResult(){
		return result;
	}
}
class ReceiveMatrixOrder extends Thread{
	Socket sock;
	int r1,c1,r2,c2;
	Ints i;
	public ReceiveMatrixOrder(Socket sock){
		this.sock = sock;
	}
	public Ints receiveIt(){
		i = new Ints();
		i.r1 = r1;
		i.r2 = r2;
		i.c1 = c1;
		i.c2 = c2;
		return i;
	}
	public void run(){
		try{
			Thread.sleep(1000);
			System.out.println("Receiving order of Matrices...");
			DataInputStream indata = new DataInputStream(sock.getInputStream());
	         		r1 = indata.readInt();
	         		System.out.println("Receiving Row1 = "+r1);
	         		Thread.sleep(500);
	         		c1 = indata.readInt();
	         		System.out.println("Receiving Col1 = "+c1);
	         		Thread.sleep(500);
	         		r2 = indata.readInt();
	         		System.out.println("Receiving Row2 = "+r2);
	         		Thread.sleep(500);
	         		c2 = indata.readInt();
	         		System.out.println("Receiving Col2 = "+c2);
	         		Thread.sleep(500);
	         	}catch(Exception e){

	         	}
	}
}
class ReceiveData extends Thread{
	int mat1[][],mat2[][],result [][];
	Socket sock;
	Ints i;
	public ReceiveData(Socket sock,Ints i){
		this.sock = sock;
		this.i = i;
	}
	public void run(){
		try{
			mat1 = new int[i.r1][i.c1];
			mat2 = new int[i.r2][i.c2];
			result = new int[i.r1][i.c2];
			int row = 0;
			DataInputStream indata = new DataInputStream(sock.getInputStream());
			while(row<i.r1){
				System.out.println("Receiving Row "+row+" of Matrix 1");
				Thread.sleep(1000);
				System.out.print("Receiving ");
				for(int c=0;c<i.c1;c++){
					int element = indata.readInt();
					mat1[row][c] = element;
					System.out.print(element+ " ");
					Thread.sleep(1000);
				}
				System.out.println("\nReceiving Complete Matrix2: ") ;
				int r = i.r2,c=i.c2;
				for(int i=0;i<r;i++){
					for(int j=0;j<c;j++){
						int element = indata.readInt();
						mat2[i][j] = element;
						System.out.print(element+ " ");
						Thread.sleep(1000);
					}
					System.out.println();
				}
				for(int col=0;col<i.c2;col++){
					ComputeAnElement compute = new ComputeAnElement(result,i,row,col,mat1,mat2);
					compute.start();
					result = compute.updateResult();
				}
				row++;
				System.out.println();
			}
		}catch(Exception e){
			System.out.println(e);
		}
	}
	public int[][] getResult(){
		return result;
	}
}
class Server{
	public static Socket connect(){
		try{
			ServerSocket server = new ServerSocket(60134);
	        		System.out.println("Server Ready! Waiting for connection...");
	        		Socket sock = server.accept();
	       	 	System.out.println("Connection received from " + sock.getInetAddress().getHostName() + " : " + sock.getPort());
	       	 	return sock;
       	 	}catch(Exception e){
			System.out.println(e);
			return null;
		}
	}
	public static void main(String str[]){
		int r1,r2,c1,c2;
		int [][]result;
		try{
	        		Socket sock = connect();
	        		ReceiveMatrixOrder rcv_order = new ReceiveMatrixOrder(sock);
	         		rcv_order.start();
	         		rcv_order.join();
	         		Ints i = rcv_order.receiveIt();
	         		r1 = i.r1;
	         		r2 = i.r2;
	         		c1 = i.c1;
	         		c2 = i.c2;
	         		ReceiveData rcv_data = new ReceiveData(sock,i);
	         		rcv_data.start();
	         		rcv_data.join();
	         		result = rcv_data.getResult();
	         		
	         		DataOutputStream outdata = new DataOutputStream(sock.getOutputStream());
	         		System.out.println("Sending Result: ");
	         		
	         		System.out.println("Sending ");
			for(int row=0;row<i.r1;row++){
				for(int col=0;col<i.c2;col++){
					System.out.print(result[row][col]+" ");
					outdata.writeInt((int)result[row][col]);
					//outdata.flush();
					Thread.sleep(1000);
				}
				System.out.println();
			}
		}catch(Exception e){
			System.out.println(e);
		}
	}
}
