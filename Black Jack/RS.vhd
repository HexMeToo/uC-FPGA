library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.std_logic_unsigned.all;

entity RS is
	port(	CLK: in std_logic;
			S: in std_logic;
			R: in std_logic;
			Q: buffer std_logic);
end RS;

architecture Behavioral of RS is
begin
	process(clk)
	begin
		if S = '1' and R = '1' then
			Q <= Q;
		end if;
		if S = '1' and R = '0' then
			Q <= '0';
		end if;
		if S = '0' and R = '1' then
			Q <= '1';
		end if;
	end process;
end Behavioral;