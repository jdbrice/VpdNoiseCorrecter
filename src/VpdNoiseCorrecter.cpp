#include "VpdNoiseCorrecter.h"


void VpdNoiseCorrecter::preEventLoop(){
	TreeAnalyzer::preEventLoop();
	INFO( classname(), "Finding Centers" );


	int total = chain->GetEntries();
	for ( int iEvent = 0; iEvent < total; iEvent++ ){

		chain->GetEvent( iEvent );

		for ( int iTube = 0; iTube < 16; iTube++ ){

			int eastTdc = ds->get<int>( "tdcEast", iTube );
			int eastAdc = ds->get<int>( "adcEast", iTube );

			if ( eastAdc > 10 && eastTdc > 300 )
				book->fill( "tdcEast", iTube, eastTdc );
			
			int westTdc = ds->get<int>( "tdcWest", iTube );
			int westAdc = ds->get<int>( "adcWest", iTube );

			if ( westAdc > 10 && westTdc > 300 )
				book->fill( "tdcWest", iTube, westTdc );
		}


		book->fill( "fastTdcEast", ds->get<int>( "fastChEast" ), ds->get<int>( "fastTdcEast" ) );
		book->fill( "fastTdcWest", ds->get<int>( "fastChWest" ), ds->get<int>( "fastTdcWest" ) );
	}

	for ( int iTube = 0; iTube < 16; iTube++ ){

		TH1D * py = book->get2D( "tdcEast" )->ProjectionY( "py", iTube+1, iTube+1 );
		TF1 * fg = new TF1( "ggaus", "gaus" );
		py->GetXaxis()->SetRangeUser( py->GetMean() - 40, py->GetMean() + 40 );
		py->Fit( fg );
		centerEast[ iTube ] = fg->GetParameter( 1 ) - 1000;
		cout << "center[ " << iTube << " ] = " << centerEast[iTube] << endl;
		book->get( "tdcEastCenter" )->SetBinContent( iTube+1, fg->GetParameter( 1 ) );
		delete py;

		py = book->get2D( "tdcWest" )->ProjectionY( "py", iTube+1, iTube+1 );
		centerWest[ iTube ] = py->GetMean() - 1000;
		book->get( "tdcWestCenter" )->SetBinContent( iTube+1, py->GetMean() );
		delete py;
	}


	for ( int iTubeA : { 0, 4, 8, 12 } ){
		for ( int iTubeB  : { 0, 4, 8, 12 } ){
			book->clone( "deltaVz", "deltaVz_" + ts( iTubeA ) + "_" + ts( iTubeB ) );
			book->clone( "deltaVz", "deltaVz_Noise_" + ts( iTubeA ) + "_" + ts( iTubeB ) );
			book->get( "deltaVz_Noise_" + ts( iTubeA ) + "_" + ts( iTubeB ) )->Sumw2();
		}
	}

	book->cd();
	book->get( "deltaVz" )->Sumw2();
	book->get( "deltaVzOff" )->Sumw2();
	book->get( "deltaVzOffNoise" )->Sumw2();
	book->get( "deltaVzNoise" )->Sumw2();

}



void VpdNoiseCorrecter::analyzeEvent(){


	int fastEast = ds->get<int>( "fastTdcEast" );
	int fastWest = ds->get<int>( "fastTdcWest" );

	int fastAdcEast = ds->get<int>( "fastAdcEast" );
	int fastAdcWest = ds->get<int>( "fastAdcWest" );

	if ( fastEast < 300 || fastWest < 300 ) return;
	if ( fastAdcEast < 10 || fastAdcWest < 10 ) return;


	int chEast = ds->get<int>( "fastChEast" );
	int chWest = ds->get<int>( "fastChWest" );

	int pulserChEast = ds->get<int>( "fastPulserChEast" );
	int pulserChWest = ds->get<int>( "fastPulserChWest" );

	int pulserEast = ds->get<int>( "tdcEast", pulserChEast ) - centerEast[ pulserChEast ];
	int pulserWest = ds->get<int>( "tdcWest", pulserChWest ) - centerWest[ pulserChWest ];

	float L0vz = (fastWest - fastEast) * 29.979 * 0.5 * 0.01888;
	float vz = ds->get<float>( "vertexZ" );

	float offL0vz = ((fastWest - centerWest[ chWest ]) - (fastEast - centerEast[ chEast ] )) * 29.979 * 0.5 * 0.01888;
	float offNoiseL0vz = ((fastWest - centerWest[ chWest ] - pulserWest) - (fastEast - centerEast[ chEast ] - pulserEast )) * 29.979 * 0.5 * 0.01888;

	book->fill( "fastPulserChWest", pulserChWest );
	book->fill( "fastPulserChEast", pulserChEast ); 

	float noiseL0vz = ((fastWest - pulserWest) - (fastEast - pulserEast )) * 29.979 * 0.5 * 0.01888;

	book->fill( "deltaVz_" + ts( pulserChWest ) + "_" + ts( pulserChEast ), L0vz - vz );
	book->fill( "deltaVz_Noise_" + ts( pulserChWest ) + "_" + ts( pulserChEast ), noiseL0vz - vz );
	

	book->fill( "deltaVz", ( L0vz - vz ) );
	book->fill( "deltaVzOff", ( offL0vz - vz ) );
	book->fill( "deltaVzOffNoise", ( offNoiseL0vz - vz ) );
	book->fill( "deltaVzNoise", ( noiseL0vz - vz ) );

	book->fill( "tdcWestCentered", chWest, fastWest - centerWest[ chWest ] );
	book->fill( "tdcEastCentered", chEast, fastEast - centerEast[ chEast ] );


}