package chSound;

/************************************************************************************

byte to audio signal converter

This class converts bytes to bits into an audio signal with differential manchester 
coding. 

*************************************************************************************

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

********************************** list of outhors **********************************

v0.1  2.3.2017 C. -H-A-B-E-R-E-R-  initial version

It is mandatory to keep the list of authors in this code.
Please add your name if you improve/extend something
2017 ChrisMicro

*************************************************************************************
*/
import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.LineUnavailableException;
import javax.sound.sampled.SourceDataLine;


public class DataSignalGenerator 
{
	protected static final int SAMPLE_RATE = 44100;
	   
	private int     synchronizationPreampleNumberOfBytes = 3;

	private byte    manchesterPhase     =  127;    // current phase for differential manchester coding
	
	private int     manchesterNumberOfSamplesPerBit = 4; // this value must be even
	
	private SourceDataLine line;

	
	public DataSignalGenerator () throws LineUnavailableException
	{

	}

	public void open() throws LineUnavailableException
	{
		final AudioFormat af = new AudioFormat(SAMPLE_RATE, 8, 1, true, true);
		line = AudioSystem.getSourceDataLine(af);
		line.open(af, SAMPLE_RATE);
		line.start();		
	}
	
	public void close()
	{
		line.drain();
		line.close();
	}
	
	private byte[] manchesterEdge(boolean bitValue, int pointerIntoSignal, byte signal[] )
	{
		byte sigpart[]=new byte[manchesterNumberOfSamplesPerBit];
		int n;

		if(bitValue) manchesterPhase = (byte)-manchesterPhase; // toggle phase
		for(n=0;n<manchesterNumberOfSamplesPerBit;n++)
		{
			if(n==(manchesterNumberOfSamplesPerBit/2))manchesterPhase=(byte)-manchesterPhase; // toggle phase
			signal[pointerIntoSignal]=manchesterPhase;
			pointerIntoSignal++;
		}		
		
		return sigpart;
	}

	public byte[] getDifferentialManchesterCodedSignal(int hexdata[])
	{
		int laenge=hexdata.length;
		byte[] signal=new byte[(laenge*8)*manchesterNumberOfSamplesPerBit];
		
		int counter=0;

		/** create data signal **/
		for(int count=0;count<hexdata.length;count++)
		{
			int dat=hexdata[count];

			/** create one byte **/			
			for( int n=0;n<8;n++) // first bit to send: MSB
			{
				if((dat&0x80)==0) 	manchesterEdge(false,counter,signal); // low bit
				else 				manchesterEdge(true,counter,signal);  // high bit
				counter += manchesterNumberOfSamplesPerBit;	
				dat      = dat << 1; // shift to next bit
			}
		}
		return signal;	
	}
	
	void playSignal(int[] inputData) throws LineUnavailableException 
	{
		open();

		int dataLength=inputData.length;
	
		int[] data=new int[synchronizationPreampleNumberOfBytes+dataLength];

		// initialize preample with high bits ( fast pulses )
		for(int n=0;n<synchronizationPreampleNumberOfBytes;n++) data[n]=0;
		
		// set start bit
		data[synchronizationPreampleNumberOfBytes-1]=0x1;

		// add data bytes to stream
		for(int n=0;n<dataLength;n++)data[synchronizationPreampleNumberOfBytes+n]=inputData[n];
		
		// convert to manchester code
		byte[] sig=getDifferentialManchesterCodedSignal(data);

		// play it
		line.write(sig, 0, sig.length);
		
		close();
		
	}

}
