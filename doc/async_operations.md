# Asynchronous operations

Node.js native add-ons often need to execute long running tasks and to avoid
blocking the **event loop** they have to run them asynchronously from the
**event loop**.
In the Node.js model of execution there are **two threads**, the first is the
**event loop** thread, that represents the thread where JavaScript code is
executing. The node.js guidance is to avoid you blocking other work queued on the
event loop by. Therefore, we need to do this work on another thread.

All this means that native add-ons need to leverage async helpers from libuv as
part of their implementation. This allows them to schedule work to be executed
asynchronously so that their methods can return in advance of the work being
completed.

Node Addon API provides an ABI-stable interface to support functions which covers
the most common asynchronous use cases. You have two abstract class to implement
asynchronous operation:

- **[AsyncWorker](async_worker.md)**
- **[Async Context](async_context.md)**

These two classes help you to manage asynchronous operations through an abstraction
of the concept of moving data between the **event loop** and **worker threads**.