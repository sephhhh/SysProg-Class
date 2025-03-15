1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

The remote client manages partial readings with buffering and retry logic and uses delimiters, length-prefixing, or connection closure to decide when a command's output ends.

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

Commands may be merged or divided erroneously if a networked shell protocol does not employ length-prefixing or delimiters (such as newline or special character) to specify message boundaries.

3. Describe the general differences between stateful and stateless protocols.

Stateless protocols handle each request separately without keeping track of past interactions, whereas stateful protocols save session information throughout several sessions.

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

When minimal latency is essential, such in real-time streaming or gaming, when a little packet loss is tolerable in comparison to TCP's overhead, UDP is utilized.

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

Applications may connect to networks and transmit and receive data using a variety of protocols thanks to the socket API that the operating system exposes.
