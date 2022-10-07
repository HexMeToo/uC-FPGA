library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.std_logic_unsigned.all;

entity Mux is
	port( 
			A:	in std_logic_vector(7 downto 0);
			B:	in std_logic_vector(7 downto 0);
			W: in std_logic;
			Q: out std_logic_vector(7 downto 0));
end Mux;

architecture Behavioral of Mux is
begin
	process(W)
	begin
--		if CLK'event and CLK = '1' then
			if W = '1' then
				Q <= A;
			else
				Q <= B;
			end if;
--		end if;
	end process;
	
end Behavioral;