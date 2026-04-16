-- Inventory Management System - Supabase SQL Setup
-- This script creates tables and sample data for a basic inventory flow.

create extension if not exists "pgcrypto";

create table if not exists public.products (
  id uuid primary key default gen_random_uuid(),
  product_code text unique not null,
  name text not null,
  category text not null check (category in ('electronics', 'food', 'office')),
  supplier text not null,
  unit_price numeric(12,2) not null check (unit_price >= 0),
  quantity integer not null default 0 check (quantity >= 0),
  min_stock integer not null default 5 check (min_stock >= 0),
  brand text,
  warranty_months integer,
  expiry_date date,
  storage_info text,
  material text,
  reusable boolean,
  created_at timestamptz not null default now(),
  updated_at timestamptz not null default now()
);

create table if not exists public.stock_transactions (
  id bigserial primary key,
  product_id uuid not null references public.products(id) on delete cascade,
  action text not null check (action in ('RESTOCK', 'SALE', 'CREATE', 'DELETE')),
  quantity integer not null check (quantity > 0),
  unit_price numeric(12,2) not null check (unit_price >= 0),
  total_price numeric(12,2) generated always as (quantity * unit_price) stored,
  note text,
  created_at timestamptz not null default now()
);

create or replace function public.set_updated_at()
returns trigger
language plpgsql
as $$
begin
  new.updated_at = now();
  return new;
end;
$$;

drop trigger if exists trg_products_updated_at on public.products;
create trigger trg_products_updated_at
before update on public.products
for each row
execute function public.set_updated_at();

create index if not exists idx_products_category on public.products(category);
create index if not exists idx_products_quantity on public.products(quantity);
create index if not exists idx_transactions_product_id on public.stock_transactions(product_id);
create index if not exists idx_transactions_created_at on public.stock_transactions(created_at desc);

alter table public.products enable row level security;
alter table public.stock_transactions enable row level security;

drop policy if exists products_select_anon on public.products;
create policy products_select_anon
on public.products
for select
to anon
using (true);

drop policy if exists products_insert_anon on public.products;
create policy products_insert_anon
on public.products
for insert
to anon
with check (true);

drop policy if exists products_update_anon on public.products;
create policy products_update_anon
on public.products
for update
to anon
using (true)
with check (true);

drop policy if exists stock_transactions_select_anon on public.stock_transactions;
create policy stock_transactions_select_anon
on public.stock_transactions
for select
to anon
using (true);

drop policy if exists stock_transactions_insert_anon on public.stock_transactions;
create policy stock_transactions_insert_anon
on public.stock_transactions
for insert
to anon
with check (true);

-- Optional view for quick dashboard usage
create or replace view public.v_low_stock as
select
  p.id,
  p.product_code,
  p.name,
  p.category,
  p.quantity,
  p.min_stock,
  (p.min_stock - p.quantity) as missing_qty,
  p.updated_at
from public.products p
where p.quantity <= p.min_stock;

insert into public.products
(product_code, name, category, supplier, unit_price, quantity, min_stock, brand, warranty_months, expiry_date, storage_info, material, reusable)
values
('ELEC-001', 'Laptop', 'electronics', 'TeknoMarket', 37999.90, 8, 5, 'Lenovo', 24, null, null, null, null),
('FOOD-001', 'Milk', 'food', 'FreshFarm', 32.50, 3, 5, null, null, '2026-05-01', 'Keep refrigerated', null, null),
('OFF-001', 'Notebook', 'office', 'OfficePlus', 24.75, 12, 6, null, null, null, null, 'Paper', true)
on conflict (product_code) do nothing;

insert into public.stock_transactions (product_id, action, quantity, unit_price, note)
select id, 'CREATE', quantity, unit_price, 'Initial seed record'
from public.products
where product_code in ('ELEC-001', 'FOOD-001', 'OFF-001')
on conflict do nothing;

-- Note: Demo icin anon policy acik tutuldu. Uretimde stricter policy kullanin.
