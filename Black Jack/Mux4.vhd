library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.std_logic_unsigned.all;

entity Mux4 is
	port( 
			A:	in std_logic_vector(3 downto 0);
			B:	in std_logic_vector(3 downto 0);
			C:	in std_logic_vector(3 downto 0);
			D:	in std_logic_vector(3 downto 0);
			Addr: in std_logic_vector(1 downto 0);
			Q: out std_logic_vector(3 downto 0));
end Mux4;

architecture Behavioral of Mux4 is
begin
	process(Addr)
	begin
		case Addr is
			when "00" => Q <= A;
			when "01" => Q <= B;
			when "10" => Q <= C;
			when "11" => Q <= D;
		end case;
	end process;
	
end Behavioral;