#ifndef rosic_ConvolverPartitioned_h
#define rosic_ConvolverPartitioned_h

// rosic-indcludes:
#include "rosic_ConvolverBruteForce.h"
#include "rosic_ConvolverFFT.h"

namespace rosic
{

  /**

  This class implements a convolution via partioning the impulse response into several blocks and
  convolving the input signal with each of these blocks separately. In order to achieve a zero
  input/output delay, convolution with the first block will be realized by direct convolution and 
  later blocks will be computed by FFT/IFFT. The algorithms used here partitions the impulse 
  response in a way which minimizes the the total computation per sample but does not ensure 
  uniform CPU load. Best efficiency in relation to the impulse response length is achieved when the 
  length is a power of two, the most wasteful case occurs at a length of a power of two plus one.

  \todo: use a more proper size for the last fftConvolver and delay it's output appropriately

  */

  class ConvolverPartitioned
  {

  public:

    //---------------------------------------------------------------------------------------------
    // construction/destruction:

    /** Constructor. */
    ConvolverPartitioned();

    /** Destructor. */
    ~ConvolverPartitioned();

    //---------------------------------------------------------------------------------------------
    // setup:

    /** Sets up the impulse response to be used. */
    void setImpulseResponse(double *newImpulseResponse, int newLength);

    //---------------------------------------------------------------------------------------------
    // audio processing:

    /** Calculates a single convolved output-sample. */
    INLINE double getSample(double in);

    //---------------------------------------------------------------------------------------------
    // others:

    /** Sets the impulse response to an impulse (1 0 0 0....). */
    void clearImpulseResponse();

    /** Sets the buffer for the previous input samples to zero. */
    void clearInputBuffers();

    //=============================================================================================

  protected:

    static const int directConvolutionLength = 64;  // \todo: check whether 64 is the optimal value

    ConvolverBruteForce   directConvolver; // the single direct convolver for the first block
    ConvolverFFT          *fftConvolvers;  // the FFT based convolvers
    int numFftConvolvers;                  // number of FFT based block convolvers
    int M;                                 // length of the impulse response
    MutexLock mutex;                       // mutex-lock for accessing the fftConvolvers 

  };

  //-----------------------------------------------------------------------------------------------
  // from here: definitions of the functions to be inlined, i.e. all functions which are supposed
  // to be called at audio-rate (they can't be put into the .cpp file):

  INLINE double ConvolverPartitioned::getSample(double in)
  {
    mutex.lock();
    double tmp = directConvolver.getSample(in);
    for(int c=0; c<numFftConvolvers; c++)
      tmp += fftConvolvers[c].getSample(in);
    mutex.unlock();

    return tmp;
  }

} // end namespace rosic

#endif // rosic_ConvolverPartitioned_h
