# Inventory Management System

This project is a simple Inventory Management System that I prepared for COME 202 - Object Oriented Programming II.

I kept the codebase intentionally straightforward. My main goal was to show the required OOP topics in a working C++ program, not to build a complex production system.

## What It Does

- Add products
- List products
- Make sales
- Restock items
- Remove products
- Show a low stock report
- View transaction history
- Save and load inventory data

## OOP Concepts Used

- Inheritance
- Polymorphism
- Virtual functions
- Constructors and destructor usage
- Function overloading
- Const methods
- Static data members and static methods
- Friend function
- Pointer usage
- Dynamic memory allocation with `new` / `delete`

## Build and Run

```bash
g++ -std=c++17 -Iinclude src/main.cpp src/Inventory.cpp -o inventory_system
./inventory_system
```

or

```bash
cmake -S . -B build
cmake --build build
./build/inventory_system
```

## Environment Variables

The project includes a `.env.example` file with Supabase variables for possible future integration.
