# Asynchronous operations

Node.js native add-ons often need to execute long running tasks and to avoid of
blocking the **event loop** they have to accomplish to them in the asynchronous way.
Lets do a quick overview of how asynchronous code work in Node.js. In our model
of execution we have **two threads**, the first is the **event loop** thread, that
represents the thread where or JavaScript code is executing in. We want avoid to 
stall the event loop thread doing heavy computation so we need to create a sencond
thread called **worker thread** managed by **libuv**, the libary that supports the
asynchronous I/O in Node.js.

All this means that native add-ons need to leverage async helpers from libuv as
part of their implementation. This allows them to schedule work to be executed
asynchronously so that their methods can return in advance of the work being
completed.

Node Addon API provides an ABI-stable interface to support functions which covers
the most common asynchronous use cases. You have two abstract class to implement
asynchronous operation:

- [AsyncWorker](async_worker.md)
- [Async Context](async_context.md)

These two classes help you to manage asynchronous operations through an abstraction
of the concept of moving data between the **event loop** and **worker threads**.