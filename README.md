go-traceroute
=============

A traceroute variant in Go.

This project is a way for me to familiarize myself with Go, and to learn more
about networking. This idea started when I was learning about DNS root servers
in a networks class. I ran some traceroutes to DNS root servers to see which I
was talking to. However, the root server itself wouldn't response to a
traceroute packet. I tried various different kinds of packets before realizing
that I should just send a DNS query packet. I decided I would implement this
myself as a means to learn Go and networking; and to achieve my goal of
tracerouting to a DNS root server.
