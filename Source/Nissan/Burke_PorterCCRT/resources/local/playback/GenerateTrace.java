import java.io.*;
import java.util.*;
import java.util.List;
import java.awt.*;

class DPoint {
	public double x;
	public double y;
	public DPoint(double a, double b) {
		x = a;
		y = b;
	}
}


public class GenerateTrace {
	public static final int HERTZ = 100;
	public static final double VARY = 10;
	public static final double PRECISION = 1.0;
	
	List keyPoints;
	public List allPoints;
	int hertz;
	static Random rand;	
	
	public GenerateTrace(List pts, int freq) {
		keyPoints = pts;
		hertz = freq;
		rand = new Random();
		
		allPoints = new ArrayList();
		
		// Loop thru all the key points (except the last one)
		// and process in pairs.
		List tempPoints;
		for (int i = 0; i < keyPoints.size() - 1; i++) {
			tempPoints = interpolate(
					(DPoint)keyPoints.get(i), (DPoint)keyPoints.get(i+1));
			if (tempPoints != null) {
				allPoints.addAll(tempPoints);
			}
		}
		// Append final point
		DPoint fin = (DPoint)keyPoints.get(keyPoints.size()-1);
		allPoints.add(new DPoint(fin.x * hertz, fin.y));
	}

	List interpolate(DPoint p1, DPoint p2) {
		List results = new ArrayList();
		double slope = (p2.y - p1.y) / ((p2.x - p1.x) * hertz);
		double yint = p1.y - (slope * p1.x * hertz);
		//System.err.println("Interpolating: " + p1.x +","+p1.y + " and " + p2.x
		//		+ "," + p2.y);
		
		for (double i = p1.x * hertz; i < p2.x * hertz; i++) {
			// y = m*x + b
			//System.err.println("x=" + i + ", y=" + (slope*i + yint));

			//double y = ((int)((slope * i + yint) * 100)) / 100.0;
			double y = slope * i + yint;
			results.add(new DPoint(i, y));
		}
		return results;
	}

	static double vary(double val, double fraction) {
		double choose = rand.nextDouble() * (val * fraction);
		return choose + val - (val * fraction / 2);
	}

	static double truncate(double val, double prec) {
		return ((int)(val * prec)) / prec;
	}
	
	public static void printDriveCurveData(
			List lfBrake, List rfBrake,	List lrBrake,	List rrBrake,
			List lfSpeed, List rfSpeed, List lrSpeed, List rrSpeed)
	 throws Exception {

		DPoint lfb, rfb, lrb, rrb;
		DPoint lfs, rfs, lrs, rrs;

		System.out.println("## Synthetic drive curve");
		for (int i = 0; i < lfBrake.size(); i++) {
			lfb = (DPoint)lfBrake.get(i);
			rfb = (DPoint)rfBrake.get(i);
			lrb = (DPoint)lrBrake.get(i);
			rrb = (DPoint)rrBrake.get(i);

			lfs = (DPoint)lfSpeed.get(i);
			rfs = (DPoint)rfSpeed.get(i);
			lrs = (DPoint)lrSpeed.get(i);
			rrs = (DPoint)rrSpeed.get(i);
			
			//System.out.print((int)lfb.x + " ");
			
			System.out.print( (int)vary(lfb.y, VARY) + " ");
			System.out.print( (int)vary(rfb.y, VARY) + " ");
			System.out.print( (int)vary(lrb.y, VARY) + " ");
			System.out.print( (int)vary(rrb.y, VARY) + "  ");
			
			System.out.print(truncate(vary(lfs.y, VARY), 100) + " ");
			System.out.print(truncate(vary(rfs.y, VARY), 100) + " ");
			System.out.print(truncate(vary(lrs.y, VARY), 100) + " ");
			System.out.print(truncate(vary(rrs.y, VARY), 100));

			System.out.print("\n");
		}
	}
	
	public void printBladeData() throws Exception {
		System.out.println("## Synthetic TPS (blade-angle) data");
		System.out.println("## " + (new Date()));
		Iterator it = allPoints.iterator();
		while (it.hasNext()) {
			DPoint point = (DPoint)it.next();
			System.out.println("00:00:" + (int)(1000/HERTZ * point.x) + " TPSValue "
					+ truncate(point.y, PRECISION));
		}
	}

	public void printPowerSupplyData(String tagname) throws Exception {
		System.out.println("## Synthetic power supply data for " + tagname);
		System.out.println("## " + (new Date()));
		
		int hertz = 2;
		double vary = 0.018;  // + or minus .9 percent
		Iterator it = allPoints.iterator();
		while (it.hasNext()) {
			DPoint point = (DPoint)it.next();

			// At this time point, generate a data point for each tag
			System.out.println("00:00:" + (1000/hertz * point.x) + " "
					+ tagname + " " + truncate(vary(point.y, vary), PRECISION) );

		}
	}
	
	public static void createDriveCurve() throws Exception {

		// Make 8 columns
		List lfSpeed = getWheelSpeedCurve(1);
		List rfSpeed = getWheelSpeedCurve(2);
		List lrSpeed = getWheelSpeedCurve(3);
		List rrSpeed = getWheelSpeedCurve(4);

		List lfBrake = getWheelBrakeCurve(1);
		List rfBrake = getWheelBrakeCurve(2);
		List lrBrake = getWheelBrakeCurve(3);
		List rrBrake = getWheelBrakeCurve(4);
		
		// Print the columns
		printDriveCurveData(
				lfBrake, rfBrake, lrBrake, rrBrake,
				lfSpeed, rfSpeed, lrSpeed, rrSpeed);
	
	}
	
	public static List getWheelBrakeCurve(double dip) throws Exception {
		List pts = new ArrayList();
		pts.add(new DPoint(0,0));
		pts.add(new DPoint(110,0));
		pts.add(new DPoint(112,20));
		pts.add(new DPoint(113, 300));

		if (dip == 1) {
			pts.add(new DPoint(113.5, 0));
			pts.add(new DPoint(114, 300));
		} else if (dip == 2) {
			pts.add(new DPoint(114, 300));
			pts.add(new DPoint(114.5, 0));
			pts.add(new DPoint(115, 300));
		} else if (dip == 3) {
			pts.add(new DPoint(115,300));
			pts.add(new DPoint(115.5,0));
			pts.add(new DPoint(116,300));
		} else if (dip == 4) {
			pts.add(new DPoint(116,300));
			pts.add(new DPoint(116.5,0));
			pts.add(new DPoint(117,300));
		}

		pts.add(new DPoint(118,300));
		pts.add(new DPoint(118.5,0));

		pts.add(new DPoint(123,0));
		pts.add(new DPoint(123.5, 300));
		pts.add(new DPoint(126, 300));
		pts.add(new DPoint(126.5, 0));
		pts.add(new DPoint(150,0));
		
		return (new GenerateTrace(pts, HERTZ)).allPoints;	
	}
	
	public static List getWheelSpeedCurve(double plat) throws Exception {
		List pts = new ArrayList();
		pts.add(new DPoint(0,0));
		pts.add(new DPoint(30,0));
		pts.add(new DPoint(36, -10));
		pts.add(new DPoint(45,0));
		pts.add(new DPoint(57,15));

		pts.add(new DPoint(67,4));
		pts.add(new DPoint(72,50));
		pts.add(new DPoint(80,53));
		pts.add(new DPoint(84,52));
		pts.add(new DPoint(88,48));
		pts.add(new DPoint(92,54));
		pts.add(new DPoint(96,54));
		pts.add(new DPoint(98,49));
		pts.add(new DPoint(104,49));
		pts.add(new DPoint(106,60));
		pts.add(new DPoint(108,65));
		pts.add(new DPoint(110,66));
		DPoint startDescent = new DPoint(112,60);
		DPoint stopDescent = new DPoint(118,0);
		pts.add(startDescent);
		
		// Plataue
		/*
	 	double holdspeed = Math.abs( ((plat - startDescent.x)
	 			 * (stopDescent.y - startDescent.y))
	 			/ (stopDescent.x - startDescent.x) );
	 	pts.add(new DPoint(startDescent.x + plat, holdspeed));
	 	pts.add(new DPoint(startDescent.x + plat + 1, holdspeed));
		*/
		pts.add(stopDescent);
		pts.add(new DPoint(120,0));
		pts.add(new DPoint(123,15));
		pts.add(new DPoint(126,0));
		pts.add(new DPoint(150,0));
		return (new GenerateTrace(pts, HERTZ)).allPoints;	
	}

	
	public static void createBladeData() throws Exception {
		List pts = new ArrayList();
		pts.add(new DPoint(66,0));
		pts.add(new DPoint(68,45));
		pts.add(new DPoint(69,49));
		pts.add(new DPoint(70,42));
		pts.add(new DPoint(72,45));
		pts.add(new DPoint(73,0));
		GenerateTrace tr = new GenerateTrace(pts, HERTZ);
		tr.printBladeData();
	}

	public static void createPowerSupplyData() throws Exception {
		List pts = new ArrayList();
		pts.add(new DPoint(0,12));
		pts.add(new DPoint(150,12));
		GenerateTrace tr = new GenerateTrace(pts, 2);
		tr.printPowerSupplyData("UpperPos12VoltPowerSupply");
		tr.printPowerSupplyData("LowerPos12VoltPowerSupply");

		pts = new ArrayList();
		pts.add(new DPoint(0,5));
		pts.add(new DPoint(150,5));
		tr = new GenerateTrace(pts, 2);
		tr.printPowerSupplyData("UpperPos5VoltPowerSupply");
		tr.printPowerSupplyData("LowerPos5VoltPowerSupply");

		pts = new ArrayList();
		pts.add(new DPoint(0,-12));
		pts.add(new DPoint(150,-12));
		tr = new GenerateTrace(pts, 2);
		tr.printPowerSupplyData("UpperNeg12VoltPowerSupply");
		tr.printPowerSupplyData("LowerNeg12VoltPowerSupply");
	}
	
	static void randomSeries() {
		Random rand = new Random();
		// 12000 samples * 4 series
		for (int i = 0; i < 12000; i++) {
			for (int  j = 0; j < 4; j++) {
				int val = (int)(rand.nextDouble() * 100);
				System.out.print(val + " ");
			}
			System.out.print("\n");
		}
	}
	
	public static void main(String args[]) throws Exception {
	//	createBladeData();
		//createPowerSupplyData();
//		createDriveCurve();
		randomSeries();
	}
}
