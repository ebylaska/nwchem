// $Id: whichjava.java,v 1.2 1999-09-20 22:02:14 d3j191 Exp $

public class whichjava implements Runnable {
  public static void main(String[] args){
    System.out.println("The Java Version in your path is " + System.getProperty("java.version"));
    if (System.getProperty("java.version").indexOf("1.2")>=0){System.exit((int)0);};
    System.out.println (" This code requires Java 1.2 or greater"); System.exit((int)1);
  }

  public void init(){};

  public void start(){};

  public void run(){};

}

